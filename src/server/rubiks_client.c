
#include "rubiks_client.h"

#include <ncurses.h>
#include <netinet/in.h>  // sockaddr_in
#include <pthread.h>
#include <stdio.h>       // getline
#include <stdlib.h>      // free
#include <sys/socket.h>  // connect, sockaddr

#include "../constants.h"
#include "../rubiks_start.h"
#include "encoding.h"
#include "server_util.h"

#define CLIENT_PRINT_DEBUG 0

void try_connect(int client_socket, struct sockaddr_in server_addr) {
  if (connect(client_socket, (struct sockaddr*)&server_addr,
              sizeof(server_addr)) == -1) {
    error_and_exit("Client failed to bind to server.");
  };
}

FILE* get_socket_file(int client_socket) {
  FILE* socket_file = fdopen(client_socket, "w+");
  if (socket_file == NULL) {
    error_and_exit("Couldn't open socket as file stream");
  }
  return socket_file;
}

client_thread_args* make_client_thread_args(int socket_file_descriptor) {
  client_thread_args* thread_args =
      (client_thread_args*)malloc(sizeof(client_thread_args));
  if (thread_args == NULL) {
    error_and_exit("Failed to create client_thread_args object");
  }
  int* cube_face_id_ptr = (int*)malloc(sizeof(int));
  if (cube_face_id_ptr == NULL) {
    error_and_exit("Failed to create cube_face_id_ptr");
  }
  sem_t* update_view_semaphore = (sem_t*)malloc(sizeof(sem_t));
  if (update_view_semaphore == NULL) {
    error_and_exit("Failed to create semaphore object");
  }
  sem_init(update_view_semaphore, 0, 0);
  thread_args->socket_file_descriptor = socket_file_descriptor;
  thread_args->cube_face_id_ptr = cube_face_id_ptr;
  thread_args->update_view_semaphore = update_view_semaphore;
  thread_args->cube_state_to_display = make_cube();
  return thread_args;
}

void free_client_thread_args(client_thread_args* thread_args) {
  free(thread_args->cube_face_id_ptr);
  free(thread_args->cube_state_to_display);
  sem_destroy(thread_args->update_view_semaphore);
  free(thread_args);
}

int run_cube(int socket_file_descriptor) {
  // Start curses
  initscr();
  noecho();
  cbreak();
  start_color();
  clear();
  refresh();

  // start two threads:
  // One runs curses and writes the result to the socket.
  // The other listens to the socket and calls show_cube when it receives a
  // message.

  client_thread_args* thread_args =
      make_client_thread_args(socket_file_descriptor);

  // launch socket listener thread
  pthread_t view_updater = 0;
  if (CLIENT_PRINT_DEBUG) {
    printf("Creating Thread #0\n");
  }
  if (pthread_create(&view_updater, NULL, listen_to_semaphore_and_update_view,
                     (void*)thread_args) != 0) {
    error_and_exit("Error creating curses listener thread");
  }

  // launch socket listener thread
  pthread_t socket_listener = 0;
  if (CLIENT_PRINT_DEBUG) {
    printf("Creating Thread #1\n");
  }
  if (pthread_create(&socket_listener, NULL,
                     listen_to_socket_and_signal_semaphore_to_view,
                     (void*)thread_args) != 0) {
    error_and_exit("Error creating socket listener thread");
  }

  // launch socket listener thread
  pthread_t curses_listener = 0;
  if (CLIENT_PRINT_DEBUG) {
    printf("Creating Thread #2\n");
  }
  if (pthread_create(&curses_listener, NULL,
                     listen_to_curses_and_send_message_to_socket,
                     (void*)thread_args) != 0) {
    error_and_exit("Error creating curses listener thread");
  }

  // wait for both threads to finish
  if (pthread_join(curses_listener, NULL) != 0) {
    error_and_exit("Curses thread failed to join");
  }
  if (pthread_join(socket_listener, NULL) != 0) {
    error_and_exit("Socket thread failed to join.");
  }
  return 0;
}

void* listen_to_curses_and_send_message_to_socket(void* thread_args_void_ptr) {
  client_thread_args* thread_args = (client_thread_args*)thread_args_void_ptr;
  FILE* socket_write = fdopen(thread_args->socket_file_descriptor, "we");
  if (socket_write == NULL) {
    error_and_exit("Failed to open socket file for writing");
  }
  int input = 0;
  char msg[N_CHARACTERS_TO_ENCODE_MOVE];
  // Get input
  while (1) {
    input = getch();  // curses call to input from keyboard
    switch (input) {
      case 'a':
      case 'd':
        if (encode_user_input(input, *(thread_args->cube_face_id_ptr), msg) !=
            0) {
          break;
        };
        (void)fputs(msg, socket_write);
        (void)fflush(socket_write);
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
        *(thread_args->cube_face_id_ptr) = input - '0';
        sem_post(thread_args->update_view_semaphore);
        break;
      case ' ':
        if (encode_user_input(input, *(thread_args->cube_face_id_ptr), msg) !=
            0) {
          break;
        };
        (void)fputs(msg, socket_write);
        (void)fflush(socket_write);
        break;
      default:
        break;
    }
    if (input == 'q' || input == 'Q') {
      break;
    }
  }
  // Close window
  endwin();
  return NULL;
}

void* listen_to_socket_and_signal_semaphore_to_view(
    void* thread_args_void_ptr) {
  client_thread_args* thread_args = (client_thread_args*)thread_args_void_ptr;
  FILE* socket_read = fdopen(thread_args->socket_file_descriptor, "re");
  if (socket_read == NULL) {
    error_and_exit("Failed to open socket file for reading");
  }

  size_t line_size = INITIAL_LINE_SIZE;
  char* line = (char*)malloc(sizeof(char) * line_size);
  if (line == NULL) {
    error_and_exit("Malloc failed for reading line from pipe.");
  }
  if (CLIENT_PRINT_DEBUG) {
    printf("Client thread listening to socket on file descriptor %d\n",
           thread_args->socket_file_descriptor);
  }
  while (getline(&line, &line_size, socket_read) != -1) {
    if (CLIENT_PRINT_DEBUG) {
      printf("Client received message: ");
      puts(line);
    }

    // Decode cube object
    decode(thread_args->cube_state_to_display, line);
    if (CLIENT_PRINT_DEBUG) {
      print_cube(thread_args->cube_state_to_display);
    }
    sem_post(thread_args->update_view_semaphore);
  }
  if (fclose(socket_read) != 0) {
    error_and_exit("Failed to close file.");
  }
  free(line);

  return NULL;
}

void* listen_to_semaphore_and_update_view(void* thread_args_void_ptr) {
  client_thread_args* thread_args = (client_thread_args*)thread_args_void_ptr;

  // Initialize colors
  const short COLOR_ORANGE = 8;
  const short RGB_MAX = 1000;
  init_color(COLOR_ORANGE, RGB_MAX, (short)(RGB_MAX / 2), 0);
  init_pair(FACE_FRONT + 1, COLOR_BLACK, COLOR_WHITE);
  init_pair(FACE_RIGHT + 1, COLOR_BLACK, COLOR_RED);
  init_pair(FACE_TOP + 1, COLOR_BLACK, COLOR_BLUE);
  init_pair(FACE_BOTTOM + 1, COLOR_BLACK, COLOR_GREEN);
  init_pair(FACE_LEFT + 1, COLOR_BLACK, COLOR_ORANGE);
  init_pair(FACE_BACK + 1, COLOR_BLACK, COLOR_YELLOW);

  while (1) {
    sem_wait(thread_args->update_view_semaphore);
    update_curses_with_cube_state(thread_args->cube_state_to_display,
                                  *(thread_args->cube_face_id_ptr));
  }
  return NULL;
}

void update_curses_with_cube_state(cube* cube_state_to_display,
                                   int cube_face_id) {
  const int CURSOR_SPACES = 30;
  const int CURSOR_NEW_LINES = CURSOR_SPACES / 2;

  // Display cube face
  for (int row = 0; row < N_SQUARES_PER_SIDE; row++) {
    for (int col = 0; col < N_SQUARES_PER_SIDE; col++) {
      attron(COLOR_PAIR(cube_state_to_display->faces[cube_face_id]
                            ->colors[row * N_SQUARES_PER_SIDE + col] +
                        1));
      for (int current_line = 0; current_line < CURSOR_NEW_LINES - 1;
           current_line++) {
        move(row * CURSOR_NEW_LINES + current_line, col * CURSOR_SPACES);
        for (int current_char = 0; current_char < CURSOR_SPACES - 1;
             current_char++) {
          printw(" ");
        }
        printw("\n");
        refresh();
      }
    }
  }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
int encode_user_input(int input, int face_num, char* encoded_message) {
  switch (face_num) {
    case FACE_FRONT:
      encoded_message[0] = '0';
      break;
    case FACE_RIGHT:
      encoded_message[0] = '1';
      break;
    case FACE_TOP:
      encoded_message[0] = '2';
      break;
    case FACE_BOTTOM:
      encoded_message[0] = '3';
      break;
    case FACE_LEFT:
      encoded_message[0] = '4';
      break;
    case FACE_BACK:
      encoded_message[0] = '5';
      break;
    default:
      return 1;
  }
  encoded_message[N_CHARACTERS_TO_ENCODE_MOVE - 2] = '\n';
  encoded_message[N_CHARACTERS_TO_ENCODE_MOVE - 1] = '\0';
  if (input == 'a') {
    encoded_message[1] = '1';
    return 0;
  }
  if (input == 'd') {
    encoded_message[1] = '0';
    return 0;
  }
  if (input == ' ') {
    encoded_message[1] = '2';
    return 0;
  }
  return 1;
}
