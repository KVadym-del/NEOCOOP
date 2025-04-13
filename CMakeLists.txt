cmake_minimum_required(VERSION 3.10.0)
project(NEOCOOP VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Configuring ${PROJECT_NAME}...")

add_subdirectory(Server)
add_subdirectory(Client)

