# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion")

list(APPEND CMAKE_CTEST_ARGUMENTS --verbose)

add_executable(test_rubiks_start test_rubiks_start.c)
target_link_libraries(test_rubiks_start
     PRIVATE rubiks_start
     PUBLIC ${CRITERION}
 )
 add_test(
     NAME test_rubiks_start
     COMMAND test_rubiks_start ${CRITERION_FLAGS}
 )

 add_executable(test_rubiks_client test_rubiks_client.c)
target_link_libraries(test_rubiks_client
     PRIVATE rubiks_client
     PUBLIC ${CRITERION}
 )
 add_test(
     NAME test_rubiks_client
     COMMAND test_rubiks_client ${CRITERION_FLAGS}
 )

 add_executable(test_rubiks_server test_rubiks_server.c)
target_link_libraries(test_rubiks_server
     PRIVATE rubiks_server
     PUBLIC ${CRITERION}
 )
 add_test(
     NAME test_rubiks_server
     COMMAND test_rubiks_server ${CRITERION_FLAGS}
 )

 add_executable(test_server_util test_server_util.c)
 target_link_libraries(test_server_util
      PRIVATE server_util
      PUBLIC ${CRITERION}
  )
  add_test(
      NAME test_server_util
      COMMAND test_server_util ${CRITERION_FLAGS}
  )

  add_executable(test_encoding test_encoding.c)
 target_link_libraries(test_encoding
      PRIVATE encoding
      PUBLIC ${CRITERION}
  )
  add_test(
      NAME test_encoding
      COMMAND test_encoding ${CRITERION_FLAGS}
  )


# If you need to test a library called foo, use lines like the following.
# add_executable(test_foo test_foo.c)
# target_link_libraries(test_foo
#     PRIVATE foo
#     PUBLIC ${CRITERION}
# )
# add_test(
#     NAME test_foo
#     COMMAND test_foo ${CRITERION_FLAGS}
# )
