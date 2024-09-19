# If you have a program that's just a .c file and it has a main method, define
# an executable. If it has both a .c and .h file, make it a library. Also, link
# libraries appropriately here. Targets should be added in dependency order - if
# executable bar needs library foo, then you should do
#   add_library(foo foo.c foo.h)
#   add_executable(bar bar.c)
#   target_link_libraries(bar PRIVATE foo)
# or similar. The PRIVATE/INTERFACE/PUBLIC keyword will depend on whether the
# library is used only in function bodies (PRIVATE), only in function
# signatures/types (INTERFACE), or both (PUBLIC).

add_library(encoding encoding.c encoding.h)
target_link_libraries(encoding PRIVATE rubiks_start constants)

add_library(server_util server_util.c server_util.h)

add_library(rubiks_client rubiks_client.c rubiks_client.h)
target_link_libraries(rubiks_client PRIVATE server_util curses encoding rubiks_start)

add_executable(run_client run_client.c)
target_link_libraries(run_client PRIVATE rubiks_client server_util)

add_library(rubiks_server rubiks_server.c rubiks_server.h)
target_link_libraries(rubiks_server PRIVATE server_util constants rubiks_start encoding)

add_executable(run_server run_server.c)
target_link_libraries(run_server PRIVATE rubiks_server server_util constants)
