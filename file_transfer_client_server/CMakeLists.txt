cmake_minimum_required(VERSION 3.12)
project(file_transfer_client_server C)

set(CMAKE_C_STANDARD 99)

add_executable(file_transfer_client_server ftserver.c)