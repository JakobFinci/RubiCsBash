#include <arpa/inet.h>  // sockaddr_in
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "rubiks_server.h"  // echo_server, related functions
#include "rubiks_start.h"
#include "server_util.h"  // socket_address, PORT

int main(void) {
  // create server object
  struct sockaddr_in server_addr = socket_address(INADDR_ANY, PORT);

  // create the global cube state object
  cube* global_cube_state = make_cube();
  // create global cube state mutex

  pthread_mutex_t* global_cube_state_lock =
      (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  if (global_cube_state_lock == NULL) {
    error_and_exit("Error initializing global_cube_state_lock mutex");
  }
  pthread_mutex_init(global_cube_state_lock, NULL);

  // create semaphore
  sem_t* broadcast_message_ready = (sem_t*)malloc(sizeof(sem_t));
  if (broadcast_message_ready == NULL) {
    error_and_exit("Error initializing broadcast_message_ready semaphore");
  }
  sem_init(broadcast_message_ready, 0, 0);

  // instantiate server
  echo_server* server =
      make_echo_server(server_addr, BACKLOG_SIZE, global_cube_state,
                       global_cube_state_lock, broadcast_message_ready);
  // initialize thread
  pthread_t thread_wait_for_broadcast = 0;
  if (pthread_create(&thread_wait_for_broadcast, NULL,
                     server_thread_wait_and_broadcast, (void*)server) != 0) {
    error_and_exit(
        "Error creating broadcast listener thread for server pipe monitoring.");
  }

  listen_for_connections(server);
  size_t num_clients_connected = 0;
  int accept_status = 0;
  while (accept_status != -1 && num_clients_connected < N_FACES) {
    accept_status = accept_client(server, num_clients_connected);
    ++num_clients_connected;
  }

  pthread_join(thread_wait_for_broadcast, NULL);

  free_echo_server(server);
  return 0;
}
