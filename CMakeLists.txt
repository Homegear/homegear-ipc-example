cmake_minimum_required(VERSION 3.8)
project(homegear_ipc_example)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/IpcClient.cpp
        src/IpcClient.h
        src/main.cpp
        config.h)

add_library(homegear_ipc_example ${SOURCE_FILES})