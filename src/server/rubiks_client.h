#pragma once

#include <netinet/in.h>  // sockaddr_in
#include <semaphore.h>
#include <stdio.h>  // FILE

#include "../constants.h"

typedef struct {
  int socket_file_descriptor;
  int* cube_face_id_ptr;
  cube* cube_state_to_display;
  sem_t* update_view_semaphore;
} client_thread_args;

/**
 * Attempt to connect to a server on a given socket.
 *
 * Given an unconnected socket and a server address/port, attempt to connect to
 * a server. If successful, the socket descriptor client_socket can be used to
 * communicate with the server. If not, print an error message and exit without
 * returning from the function.
 *
 * @param client_socket The client's socket descriptor used for the connection.
 * @param server_addr The IPv4 socket address and port of the server.
 */
void try_connect(int client_socket, struct sockaddr_in server_addr);

/**
 * Get a file pointer for a given socket descriptor.
 *
 * Given a socket descriptor used by the client, create and return a file
 * pointer corresponding to that descriptor. This can be used with high-level
 * functions in stdio for convenience. In the event of an error in opening the
 * file stream, print an error message and exit the program, in which case this
 * function does not return.
 *
 * @param client_socket The socket descriptor to generate the file stream for.
 * @return A file pointer corresponding to client_socket.
 */
FILE* get_socket_file(int client_socket);

/**
 * Allocate memory for a client_thread_args struct with the given file
 * descriptor.
 *
 * Given an integer representing a socket file descriptor, create storage for a
 * client's thread arguments on the heap. The caller is responsible for ensuring
 * that the memory is cleaned up after use.
 *
 * @param socket_file_descriptor An integer of the file descriptor for the
 * client to use.
 * @return A pointer to the new client_thread_args struct.
 */
client_thread_args* make_client_thread_args(int socket_file_descriptor);

void free_client_thread_args(client_thread_args* thread_args);

/**
 * Send and receive a line of text with the echo server.
 *
 * Given a file pointer corresponding to the client's connection with the
 * server, read a single line of text from standard input and send it to the
 * server. Then, receive the response back and print it to standard output. In
 * the event that an error occurs when attempting to send the text to the
 * server, print an error message and exit the program, in which case this
 * function does not return. Otherwise, return an integer corresponding to
 * success or a possible error in reading text from standard input or from the
 * server (which may be an end-of-file status).
 *
 * @param socket_file The file stream descriptor used in connection with the
 * server.
 * @return 0 if the entire process succeeded, and -1 if there was an error
 * (which may be EOF).
 */
int run_cube(int socket_file_descriptor);

/**
 * Wait for a call from curses and send the message to the socket.
 *
 * Read character input through curses. If the character is 'a' or 'd', encode
 * the rotation message and send it to the server. If the character is ' ',
 * encode the shuffle message and send it to the server. If the character is a
 * number between 0-5, update the server's current face view. Essentially, this
 * is the controller in an MVC implementation.
 *
 * @param thread_args_void_ptr A thread_args pointer to the function arguments.
 * @return The null pointer.
 */
void* listen_to_curses_and_send_message_to_socket(void* thread_args_void_ptr);

/**
 * Listen to the socket and update view.
 *
 * Read the socket and notify the semaphore to update upon change. Essentially,
 * this is receiving the model in an MVC implementation.
 *
 * @param thread_args_void_ptr A thread_args pointer to the function arguments.
 * @return The null pointer.
 */
void* listen_to_socket_and_signal_semaphore_to_view(void* thread_args_void_ptr);

/**
 * Wait for the semaphore to change.
 *
 * Upon a change in the semaphore state, call the function to update the view.
 *
 * @param thread_args_void_ptr A thread_args pointer to the function arguments.
 * @return The null pointer.
 */
void* listen_to_semaphore_and_update_view(void* thread_args_void_ptr);

/**
 * Display the cube state in curses.
 *
 * Given a pointer to a cube struct and an integer representing the current
 * face, display the face on curses. Essentially, this is the view in an MVC
 * implementation.
 *
 * @param cube_state_to_display A pointer to the cube struct to display.
 * @param cube_face_id An integer representing the face index of the face to be
 * displayed (0-5).
 */
void update_curses_with_cube_state(cube* cube_state_to_display,
                                   int cube_face_id);

/**
 * Encode the user input to be passed through the socket.
 *
 * Create a message consisting of 4 characters. The first character is the face
 * index that has changed and the second character is the direction of rotation.
 *
 * @param input A character ('a', 'd', or ' ') representing the key that was
 * pressed.
 * @param face_num An integer representing the face index that was changed
 * (0-5).
 * @param encoded_message A pointer to a character array to store the encoded
 * message on.
 * @return 0 upon success and 1 if the message was not encoded.
 */
int encode_user_input(int input, int face_num, char* encoded_message);
