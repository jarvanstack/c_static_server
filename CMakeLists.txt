cmake_minimum_required(VERSION 3.19)
project(c_static_server C)

set(CMAKE_C_STANDARD 99)

add_executable(c_static_server main.c)
add_executable(static_server.c static_server.c)
add_executable(strstr_postfix_test.c strstr_postfix_test.c)
add_executable(static_server_block_2.c static_server_block_2.c)
add_executable(static_server_epoll.c static_server_epoll.c)
add_executable(file_read_test.c file_read_test.c)