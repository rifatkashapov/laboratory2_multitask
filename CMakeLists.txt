cmake_minimum_required(VERSION 3.17)
project(laboratory2)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)


add_executable(laboratory2 main.cpp)
#set(CMAKE_CXX_FLAGS "-lpthread -lrt ")