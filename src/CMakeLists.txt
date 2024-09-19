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

add_library(constants INTERFACE)
target_include_directories(constants INTERFACE .)

add_library(rubiks_start rubiks_start.c rubiks_start.h)
target_link_libraries(rubiks_start PRIVATE constants)

add_executable(rubiks_view rubiks_view.c)
target_link_libraries(rubiks_view PRIVATE rubiks_start curses constants)

add_subdirectory(server)
