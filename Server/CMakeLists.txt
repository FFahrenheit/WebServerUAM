cmake_minimum_required(VERSION 3.24)
project(HTTPServer C)

set(CMAKE_C_STANDARD 11)

add_executable(HTTPServer main.c include/Server.h src/Server.c include/WebServer.h src/WebServer.c)
