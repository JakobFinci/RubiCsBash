#include "server_util.h"

#include <stdint.h>  // uint16_t
#include <stdio.h>   // perror
#include <stdlib.h>  // exit, EXIT_FAILURE
#include <unistd.h>  // close

const uint16_t PORT = 4244;
// Change ADDRESS to the ip the server is running on
const char* const ADDRESS = "192.168.35.59";

void error_and_exit(const char* error_msg) {
  perror(error_msg);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  exit(EXIT_FAILURE);
}

int open_tcp_socket(void) {
  int socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);
  if (socket_descriptor == -1) {
    error_and_exit("Failed to open socket.");
  }
  return socket_descriptor;
}

void close_tcp_socket(int socket_) {
  if (close(socket_) == -1) {
    error_and_exit("Failed to close socket.");
  }
}

struct sockaddr_in socket_address(in_addr_t addr, in_port_t port) {
  // Replace this with your implementation.
  struct sockaddr_in sock_addr = {
      .sin_family = PF_INET, .sin_addr.s_addr = addr, .sin_port = htons(port)};
  return sock_addr;
}
