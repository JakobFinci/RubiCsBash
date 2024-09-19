#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <pthread.h>
#include <stdio.h>

#include "../src/constants.h"
#include "../src/rubiks_start.h"
#include "../src/server/rubiks_server.h"
#include "../src/server/server_util.h"

// tests from assignment 8 - none added by us yet

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

// Check that the listener socket is successfully created.
Test(make_echo_server, listener_set_correctly) {
  struct sockaddr_in addr = socket_address(INADDR_ANY, 1111);
  echo_server* server = make_echo_server(addr, BACKLOG_SIZE, NULL, NULL, NULL);
  cr_expect(ge(int, server->listener, 0));
  free_echo_server(server);
}

// Check that the address and port are set correctly.
Test(make_echo_server, addr_set_correctly) {
  struct sockaddr_in addr = socket_address(INADDR_ANY, 2222);
  echo_server* server = make_echo_server(addr, BACKLOG_SIZE, NULL, NULL, NULL);
  cr_expect(eq(u32, server->addr.sin_addr.s_addr, htonl(INADDR_ANY)));
  cr_expect(eq(u16, server->addr.sin_port, htons(2222)));
  free_echo_server(server);
}

// Check that the backlog size is set correctly.
Test(make_echo_server, backlog_set_correctly) {
  struct sockaddr_in addr = socket_address(INADDR_ANY, 3333);
  echo_server* server = make_echo_server(addr, BACKLOG_SIZE, NULL, NULL, NULL);
  cr_expect(eq(int, server->max_backlog, BACKLOG_SIZE));
  free_echo_server(server);
}

// Check that the socket is accepting connections once it starts listening.
Test(listen_for_connections, socket_listening) {
  struct sockaddr_in addr = socket_address(INADDR_ANY, 4444);
  echo_server* server = make_echo_server(addr, BACKLOG_SIZE, NULL, NULL, NULL);
  listen_for_connections(server);
  int listening = 0;
  socklen_t option_len = sizeof(int);
  cr_expect(ne(int,
               getsockopt(server->listener, SOL_SOCKET, SO_ACCEPTCONN,
                          &listening, &option_len),
               -1));
  cr_expect(ne(int, listening, 0));
  free_echo_server(server);
}

// test decode_user_input
Test(decode_user_input, decodes_properly) {
  char* line = "5d";
  cube* original = make_cube();
  original->faces[5]->colors[0] = 9;
  decode_user_input(line, original);
  cr_assert(eq(int, original->faces[5]->colors[6], 9));
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
