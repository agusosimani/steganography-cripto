cmake_minimum_required(VERSION 3.14)
project(stegobmp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -pthread -Wall -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,fuzzer-no-link -fsanitize-coverage=edge,indirect-calls")

add_executable(stegobmp main.c include/main.h embed.c include/embed.h extract.c include/extract.h encryption.c include/encryption.h)
