#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
// NOLINTNEXTLINE(cert-dcl37-c, bugprone-reserved-identifier)
#define _POSIX_C_SOURCE 200809L
#endif
#define _GNU_SOURCE  // NOLINT(cert-dcl37-c, bugprone-reserved-identifier)

#include "rubiks_server.h"

#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>  // connect, sockaddr
#include <unistd.h>

#include "encoding.h"
#include "rubiks_start.h"
#include "server_util.h"

echo_server* make_echo_server(struct sockaddr_in ip_addr, int max_backlog,
                              cube* global_cube_state,
                              pthread_mutex_t* global_cube_state_lock,
                              sem_t* broadcast_message_ready) {
  echo_server* server = malloc(sizeof(echo_server));
  server->listener = open_tcp_socket();
  server->addr = ip_addr;
  server->max_backlog = max_backlog;
  server->global_cube_state = global_cube_state;
  server->global_cube_state_lock = global_cube_state_lock;
  server->broadcast_message_ready = broadcast_message_ready;

  for (size_t cube_face_id = 0; cube_face_id < N_FACES; ++cube_face_id) {
    // client -> server
    if (pipe2(server->client_pipes[cube_face_id][0], SOCK_CLOEXEC) == -1) {
      error_and_exit("Failed to create pipe for client-handling processes.");
    }
    // server -> client
    if (pipe2(server->client_pipes[cube_face_id][1], SOCK_CLOEXEC) == -1) {
      error_and_exit("Failed to create pipe for client-handling processes.");
    }
    printf("Process %lu: Pipe client to server (read): %d\n", cube_face_id,
           server->client_pipes[cube_face_id][0][0]);
    printf("Process %lu: Pipe client to server (write): %d\n", cube_face_id,
           server->client_pipes[cube_face_id][0][1]);
    printf("Process %lu: Pipe server to client (read): %d\n", cube_face_id,
           server->client_pipes[cube_face_id][1][0]);
    printf("Process %lu: Pipe server to client (write): %d\n", cube_face_id,
           server->client_pipes[cube_face_id][1][1]);
  }
  return server;
}

void free_echo_server(echo_server* server) {
  for (size_t cube_face_id = 0; cube_face_id < N_FACES; ++cube_face_id) {
    close(server->client_pipes[cube_face_id][0][0]);
    close(server->client_pipes[cube_face_id][0][1]);
    close(server->client_pipes[cube_face_id][1][0]);
    close(server->client_pipes[cube_face_id][1][1]);
  }
  close_tcp_socket(server->listener);

  free(server->global_cube_state);
  free(server->global_cube_state_lock);
  free(server->broadcast_message_ready);
  free(server);
}

void listen_for_connections(echo_server* server) {
  // bind to the server's listening port
  int reuse = 1;
  if (setsockopt(server->listener, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse,
                 sizeof(int)) == -1) {
    error_and_exit("Failed to set the reuse option on the socket");
  }
  if (bind(server->listener, &(server->addr), sizeof(server->addr)) == -1) {
    error_and_exit("Failed to bind to socket");
  }

  // listen for connections
  if (listen(server->listener, BACKLOG_SIZE) == -1) {
    error_and_exit("Server listening failed.");
  }
}

int accept_client(echo_server* server, size_t client_id) {
  struct sockaddr_storage client_addr;
  unsigned int address_size = sizeof(client_addr);
  int connection_socket =
      accept4(server->listener, (struct sockaddr*)&client_addr, &address_size,
              SOCK_CLOEXEC);
  if (connection_socket == -1) {
    error_and_exit("Failed to open socket for accepted client.");
  }

  // create new process to handle client
  pid_t pid = fork();
  switch (pid) {
    case -1:
      // the fork failed
      error_and_exit("Failed to create fork for accepted client.");
      break;
    case 0:
      // child process
      client_facing_process(connection_socket, server, client_id);
      return 0;
      break;
    default:;
      // parent process
      // client -> server write and server -> client read are not necessary for
      // the server code
      close(server->client_pipes[client_id][0][1]);
      close(server->client_pipes[client_id][1][0]);

      server_thread_listen_to_pipe_args* server_thread_args =
          make_server_thread_listen_to_pipe_args(server, client_id);

      pthread_t pipe_listener = 0;
      if (pthread_create(&pipe_listener, NULL, server_thread_listen_to_pipe,
                         (void*)server_thread_args) != 0) {
        error_and_exit(
            "Error creating pipe listener thread for server pipe monitoring.");
      }
      return 0;
  }
}

void* server_thread_wait_and_broadcast(void* server_void_ptr) {
  // cast arguments object
  echo_server* server = (echo_server*)server_void_ptr;

  // open all pipes to threads
  FILE* pipes[N_FACES];

  for (size_t client_id = 0; client_id < N_FACES; ++client_id) {
    pipes[client_id] = fdopen(server->client_pipes[client_id][1][1], "we");
    if (pipes[client_id] == NULL) {
      error_and_exit("Broadcast message: failed to open pipe.");
    }
  }
  while (1) {
    sem_wait(server->broadcast_message_ready);
    pthread_mutex_lock(server->global_cube_state_lock);
    printf("Generating encoding for cube state...\n");
    char msg[N_CHARACTERS_TO_ENCODE_CUBE];
    if (encode(server->global_cube_state, msg) != 0) {
      printf("Failed to encode cube");
    };
    printf("Message to broadcast is: ");
    puts(msg);
    pthread_mutex_unlock(server->global_cube_state_lock);
    printf("Broadcasting message.");
    for (size_t client_id = 0; client_id < N_FACES; ++client_id) {
      printf("Broadcasting message to client %lu on file descriptor %d.\n",
             client_id, server->client_pipes[client_id][1][1]);
      (void)fputs(msg, pipes[client_id]);
      (void)fflush(pipes[client_id]);
      printf("Broadcasted message.");
    }
  }
  return NULL;
}

void interpret_command(echo_server* server, const char* line) {
  printf("Interpreting line: %s", line);
  // mutex lock
  pthread_mutex_lock(server->global_cube_state_lock);
  // change the state of the cube
  decode_user_input(line, server->global_cube_state);
  // mutex unlock
  pthread_mutex_unlock(server->global_cube_state_lock);
  sem_post(server->broadcast_message_ready);
}

server_thread_listen_to_pipe_args* make_server_thread_listen_to_pipe_args(
    echo_server* server, size_t client_id) {
  server_thread_listen_to_pipe_args* thread_args =
      (server_thread_listen_to_pipe_args*)malloc(
          sizeof(server_thread_listen_to_pipe_args));
  if (thread_args == NULL) {
    error_and_exit(
        "Failed to initialize server_thread_listen_to_pipe_args object");
  }
  thread_args->server = server;
  thread_args->client_id = client_id;
  return thread_args;
}

void free_server_thread_listen_to_pipe_args(
    server_thread_listen_to_pipe_args* thread_args) {
  free(thread_args->server);
  free(thread_args);
}

void* server_thread_listen_to_pipe(void* args_void_ptr) {
  server_thread_listen_to_pipe_args* args =
      (server_thread_listen_to_pipe_args*)args_void_ptr;
  echo_server* server = args->server;
  size_t client_id = args->client_id;

  printf(
      "Server thread listen to pipe: listening to file descriptor %d because "
      "client_id is %lu\n",
      server->client_pipes[client_id][0][0], client_id);
  FILE* pipe_receiving_end = fdopen(server->client_pipes[client_id][0][0], "r");
  if (pipe_receiving_end == NULL) {
    error_and_exit(
        "Client facing thread listen to pipe: failed to open pipe file");
  }

  size_t line_size = INITIAL_LINE_SIZE;
  char* line = (char*)malloc(sizeof(char) * line_size);
  if (line == NULL) {
    error_and_exit("Malloc failed for reading line from pipe.");
  }
  while (getline(&line, &line_size, pipe_receiving_end) != -1) {
    interpret_command(server, line);
  }
  if (fclose(pipe_receiving_end) != 0) {
    error_and_exit("Failed to close file.");
  }
  free(line);
  return NULL;
}

client_facing_thread_channels* make_client_facing_thread_channels(
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    int socket_descriptor, int pipe_receiving_end, int pipe_transmitting_end) {
  client_facing_thread_channels* channels =
      (client_facing_thread_channels*)malloc(
          sizeof(client_facing_thread_channels));
  if (channels == NULL) {
    error_and_exit("Malloc failed for reading line from pipe.");
  }
  channels->socket_descriptor = socket_descriptor;
  channels->pipe_receiving_end = pipe_receiving_end;
  channels->pipe_transmitting_end = pipe_transmitting_end;
  return channels;
}

void free_client_facing_thread_channels(
    client_facing_thread_channels* channels) {
  free(channels);
}

void client_facing_process(int socket_descriptor, echo_server* server,
                           size_t client_id) {
  // server -> client write and client -> server read are not necessary for
  // the client code
  close(server->client_pipes[client_id][0][0]);
  close(server->client_pipes[client_id][1][1]);

  client_facing_thread_channels* channels = make_client_facing_thread_channels(
      socket_descriptor, server->client_pipes[client_id][1][0],
      server->client_pipes[client_id][0][1]);

  // launch pipe listener thread
  pthread_t pipe_listener = 0;
  printf("Creating client facing thread #1\n");
  if (pthread_create(&pipe_listener, NULL, client_facing_thread_listen_to_pipe,
                     (void*)channels) != 0) {
    error_and_exit(
        "Error creating pipe listener thread during client_facing_process()");
  }

  // launch socket listener thread
  pthread_t socket_listener = 0;
  printf("Creating client facing thread #2\n");
  if (pthread_create(&socket_listener, NULL,
                     client_facing_thread_listen_to_socket,
                     (void*)channels) != 0) {
    error_and_exit(
        "Error creating socket thread during client_facing_process()");
  }

  // wait for both threads to finish
  if (pthread_join(pipe_listener, NULL) != 0) {
    error_and_exit("Pipe thread failed to join");
  }
  if (pthread_join(socket_listener, NULL) != 0) {
    error_and_exit("Socket thread failed to join.");
  }
  free_client_facing_thread_channels(channels);
}

void* client_facing_thread_listen_to_pipe(void* channels_void_ptr) {
  client_facing_thread_channels* channels =
      (client_facing_thread_channels*)channels_void_ptr;
  FILE* socket_file = fdopen(channels->socket_descriptor, "w");
  if (socket_file == NULL) {
    error_and_exit(
        "Client facing thread listen to pipe: failed to open socket file");
  }
  FILE* pipe_receiving_end = fdopen(channels->pipe_receiving_end, "r");
  if (pipe_receiving_end == NULL) {
    error_and_exit(
        "Client facing thread listen to pipe: failed to open pipe file");
  }

  size_t line_size = INITIAL_LINE_SIZE;
  char* line = (char*)malloc(sizeof(char) * line_size);
  if (line == NULL) {
    error_and_exit("Malloc failed for reading line from pipe.");
  }
  puts("Goodbye world #1");
  printf(
      "Client facing thread listen to pipe: Listening to file descriptor %d\n",
      channels->pipe_receiving_end);
  while (getline(&line, &line_size, pipe_receiving_end) != -1) {
    printf("Client facing thread received broadcasted message!\n");
    (void)fputs(line, socket_file);
    (void)fflush(socket_file);
  }
  printf(
      "Client facing thread listen to pipe: Finished reading lines and "
      "exiting.");
  if (fclose(socket_file) != 0) {
    error_and_exit("Failed to close file.");
  }
  if (fclose(pipe_receiving_end) != 0) {
    error_and_exit("Failed to close file.");
  }
  free(line);
  return NULL;
}

void* client_facing_thread_listen_to_socket(void* channels_void_ptr) {
  client_facing_thread_channels* channels =
      (client_facing_thread_channels*)channels_void_ptr;
  FILE* socket_file = fdopen(channels->socket_descriptor, "r+e");
  if (socket_file == NULL) {
    error_and_exit(
        "Client facing thread listen to socket: failed to open socket file");
  }
  FILE* pipe_transmitting_end = fdopen(channels->pipe_transmitting_end, "w");
  if (pipe_transmitting_end == NULL) {
    error_and_exit(
        "Client facing thread listen to socket: failed to open pipe file");
  }

  size_t line_size = INITIAL_LINE_SIZE;
  char* line = (char*)malloc(sizeof(char) * line_size);
  if (line == NULL) {
    error_and_exit("Malloc failed for reading line from socket.");
  }
  printf("Child process is running and awaiting a message from the socket\n");
  puts("Goodbye world #2");

  while (getline(&line, &line_size, socket_file) != -1) {
    printf("Server child process thread received message from socket!");
    (void)fputs(line, pipe_transmitting_end);
    (void)fflush(pipe_transmitting_end);
  }
  if (fclose(socket_file) != 0) {
    error_and_exit("Failed to close file.");
  }
  if (fclose(pipe_transmitting_end) != 0) {
    error_and_exit("Failed to close file.");
  }
  free(line);
  return NULL;
}

void decode_user_input(const char* command, cube* global_cube_state) {
  int face_num = command[0] - '0';
  int direction = command[1] - '0';
  if (direction == 2) {
    if (is_solved(global_cube_state)) {
      shuffle(global_cube_state, MIN_DEPTH, MAX_DEPTH);
    } else {
      for (int i = 0; i < N_FACES; i++) {
        free_face(global_cube_state->faces[i]);
        global_cube_state->faces[i] = make_face(i);
      }
    }
  } else {
    rotate(global_cube_state, face_num, direction);
  }
}
