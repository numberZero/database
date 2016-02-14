cmake_minimum_required(VERSION 2.6)
project(database)

add_executable(database main.cpp)

install(TARGETS database RUNTIME DESTINATION bin)
