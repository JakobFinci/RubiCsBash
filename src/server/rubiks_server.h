#pragma once

#include <netinet/in.h>
#include <semaphore.h>
#include <sys/socket.h>

#include "../constants.h"
#include "server_util.h"

// Group the data needed for a server to run.
typedef struct {
  /// The socket descriptor to listen for connections.
  int listener;
  /// The address and port for the listener socket.
  struct sockaddr_in addr;
  /// The maximum number of clients that can be waiting to connect at once.
  int max_backlog;
  /// Pipe files for communicating between client-facing processes and the main
  /// server.
  /// client -> server read  [0][0]
  /// client -> server write [0][1]
  /// server -> client read  [1][0]
  /// server -> client write [1][1]
  int client_pipes[N_FACES][2][2];
  /// Pointer to the cube object representing the single source of truth for the
  /// game state.
  cube* global_cube_state;
  /// Mutex to protect write access to the global cube state.
  pthread_mutex_t* global_cube_state_lock;
  /// Semaphore to signal when a change has been made to the global cube state
  /// so it needs to be broadcasted to each client.
  sem_t* broadcast_message_ready;
} echo_server;

/// Arguments to be passed to server_thread_listen_to_pipe_args() during thread
/// initialization.
typedef struct {
  /// Pointer to the server object so that server attributes can be accessed.
  echo_server* server;
  /// The client id specifying which pipe the thread should listen to.
  size_t client_id;
} server_thread_listen_to_pipe_args;

/// Arguments to be passed to client_facing_thread_listen_to_socket() and
/// client_facing_thread_listen_to_pipe() during thread initialization.
typedef struct {
  /// The socket descriptor on which to listen and and send to the client.
  int socket_descriptor;
  /// The receiving end of the pipe to receive messages from the main server
  /// process.
  int pipe_receiving_end;
  /// The transmitting end of the pipe to send messages to the main server
  /// process.
  int pipe_transmitting_end;
} client_facing_thread_channels;

/**
 * Create a new echo server in dynamic memory.
 *
 * Given a socket address and a maximum backlog size, create a new echo server
 * on the heap. Since the new server (or rather, the data it stores) is
 * dynamically allocated, the caller is responsible for cleaning the server up
 * afterwards (or terminating the program and letting that take care of things).
 *
 * @param addr The IPv4 address and port that the server will listen on.
 * @param max_backlog The max number of clients that can wait to connect to the
 * server.
 * @return A pointer to the new echo server.
 */
echo_server* make_echo_server(struct sockaddr_in ip_addr, int max_backlog,
                              cube* global_cube_state,
                              pthread_mutex_t* global_cube_state_lock,
                              sem_t* broadcast_message_ready);

/**
 * Free an echo server in dynamic memory.
 *
 * Given a pointer to an echo server on the heap, free the dynamically allocated
 * memory associated with that server. Attempting to free an uninitialized
 * server, server dynamic memory that has already been freed, or a pointer to
 * memory representing anything other than an echo_server instance will result
 * in undefined behavior.
 *
 * @param server A pointer to the server to delete.
 */
void free_echo_server(echo_server* server);

/**
 * Start listening on a server for potential client connections.
 *
 * Bind the listener socket and set it to listen for client connection requests.
 * The listener socket is part of the server's internal data and does not need
 * to be opened separately. In the event that binding or listening on the socket
 * fails, print an error message and exit the program without returning.
 *
 * @param server The server to start listening on.
 */
void listen_for_connections(echo_server* server);

/**
 * Accept a new client connection and start the echo process.
 *
 * Wait until a client connection request comes in, accepting it. Once accepted,
 * fork a new process. In the child process, start the client listening threads,
 * and in the parent, start the thread which listens to the client. The server
 * program should run the function in a loop. The child will run until the
 * client is gone. In the event of an error in accepting a connection or forking
 * a new process, print an appropriate error message and accept the program.
 *
 * @param server The server to accept the connection on.
 * @param client_id Unique id from 0 to N_FACES - 1 to determine which set of
 * pipes to use for communication
 * @return 0 for the parent process and -1 for the child (echo) process.
 */
int accept_client(echo_server* server, size_t client_id);

/**
 * Create threads for communicating with the client.
 *
 * Create one thread to listen to the client socket, echoing lines read to the
 * parent server process. Create another thread to listen to the parent server,
 * echoing lines to the client. Upon an error at any point, print an error
 * message and terminate the program, in which case the function does not
 * return.
 *
 * @param socket_descriptor The socket descriptor for the client connection.
 * @param server Server object to pass to child, containing pipe information.
 * @param client_id Unique number 0-5 for each child process to determine which
 * pipe to use for communication back to the main server.
 */
void client_facing_process(int socket_descriptor, echo_server* server,
                           size_t client_id);

/**
 * Wait for updates from cube model, then send the new cube state to each
 * client.
 *
 * Given a void pointer to an echo_server, open all pipes to N_FACES number of
 * clients and wait for updates of the cube model in the form of a semaphore
 * being set. Once the semaphore is set, the cube state is assumed to have
 * changed, and this will send the cube state to each client. If pipes from
 * server to clients fail to open, the function will error and exit.
 *
 * @param server_void_ptr A void pointer to an echo server struct containing the
 * semaphore to wait on and the list of client-facing pipes to broadcast on.
 * @return NULL
 */
void* server_thread_wait_and_broadcast(void* server_void_ptr);

/**
 * Interpret a command of user input to alter the global cube state.
 *
 * Given a pointer to an echo server and a string command line, lock the global
 * cube state and interpret the command to update the cube state. Next, unlock
 * the cube state, then post a semaphore indicating that the new cube state is
 * ready. The central server thread will listen for this semaphore and send
 * the new cube state to all clients.
 *
 * @param server an echo_server pointer that holds the global cube model.
 * @param line a const char* that contains a command indicating how to alter the
 * cube model
 */
void interpret_command(echo_server* server, const char* line);

/**
 * Listen to the client on a TCP socket and echo lines to the main server.
 *
 * This thread should run on the child process for a specific client
 * socket connection. Given a client_facing_thread_channels object, perform a
 * blocking call to `getline()` on the client socket. When a line is received,
 * print that line into the pipe to the main server.
 *
 * @param channels_void_ptr void pointer to a client_facing_thread_channels,
 * containing the client socket descriptor and pipe file descriptors for
 * communicating between the parent and child server processes.
 * @return NULL
 */
void* client_facing_thread_listen_to_socket(void* channels_void_ptr);

/**
 * Listen to the main server pipe and echo lines to the client socket.
 *
 * This thread should run on the child process for a specific client
 * socket connection. Given a client_facing_thread_channels object, perform a
 * blocking call to `getline()` on the main server pipe. When a line is
 * received, print that line into the socket connection to the client.
 *
 * @param channels_void_ptr void pointer to a client_facing_thread_channels,
 * containing the client socket descriptor and pipe file descriptors for
 * communicating between the parent and child server processes.
 * @return NULL
 */
void* client_facing_thread_listen_to_pipe(void* channels_void_ptr);

/**
 * Listen to the child process and handle commands passed on from the client.
 *
 * This thread should run as a subthread on the main server process. Given a
 * server_thread_listen_to_pipe_args object, perform a blocking call to
 * `getline()` on the pipe from the child process corresponding to the client
 * id. When a line is received, interpret the command and signal that the global
 * cube state has changed. This is done with the `interpret_command()` function.
 *
 * @param channels_void_ptr void pointer to a client_facing_thread_channels,
 * containing the client socket descriptor and pipe file descriptors for
 * communicating between the parent and child server processes.
 * @return NULL
 */
void* server_thread_listen_to_pipe(void* args_void_ptr);

/**
 * Create a new server_thread_listen_to_pipe_args object.
 *
 * Package together an echo_server object and a specific client_id to specify
 * which pipes should be used from the echo_server object, such that this
 * information can be passed to the server_thread_listen_to_pipe thread
 * function. Since the new object is dynamically allocated, the caller is
 * responsible for cleaning the server up afterwards.
 *
 * @param server The echo_server object representing the main server.
 * @param client_id The specific set of pipes contained in the echo_server
 * client_pipes array that pertains to a single client.
 * @return A pointer to the new server_thread_listen_to_pipe_args object.
 */
server_thread_listen_to_pipe_args* make_server_thread_listen_to_pipe_args(
    echo_server* server, size_t client_id);

/**
 * Free a server_thread_listen_to_pipe_args in dynamic memory.
 *
 * Given a pointer to a server_thread_listen_to_pipe_args, free the dynamically
 * allocated memory associated with that thread. Attempting to free an
 * uninitialized thread, thread dynamic memory that has already been freed, or a
 * pointer to memory representing anything other than a
 * server_thread_listen_to_pipe_args instance will result in undefined behavior.
 *
 * @param server A pointer to the server_thread_listen_to_pipe_args to free.
 */
void free_server_thread_listen_to_pipe_args(
    server_thread_listen_to_pipe_args* thread_args);

/**
 * Decode and act on a command from the user
 *
 * Given an encoded command representing the users input and the face it came
 * from, decode it into an int represeting the face number and an int
 * representinf the direction to rotate. Run rotate with the global_cube_state
 * and the two ints decoded from the command.
 *
 * @param command a 2 char string representing the encoded command to be decoded
 * @param global_cube_state a cube pointer representing the cube state to be
 * altered by the users input
 */
void decode_user_input(const char* command, cube* global_cube_state);

/**
 * Create a new client_facing_thread_channels object.
 *
 * Package together the client TCP socket descriptor and pipe file descriptors
 * to the main server, such that this information can be passed to the
 * client_facing_thread_listen_to_socket() and
 * client_facing_thread_listen_to_pipe() thread functions inside the server
 * child process. Since the new object is dynamically allocated, the caller is
 * responsible for cleaning the server up afterwards.
 *
 * @param socket_descriptor The TCP socket descriptor on which to communicate to
 * the client.
 * @param pipe_receiving_end The read end of the pipe from the main server to
 * the child process (server->client_pipes[client_id][1][0]).
 * @param pipe_receiving_end The write end of the pipe from the child process to
 * the main server (server->client_pipes[client_id][0][1]).
 * @return A pointer to the new client_facing_thread_channels object.
 */
client_facing_thread_channels* make_client_facing_thread_channels(
    int socket_descriptor, int pipe_receiving_end, int pipe_transmitting_end);
