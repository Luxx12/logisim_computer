cmake_minimum_required(VERSION 4.2)
project(logisim_computer_main)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp lexer.cpp
        parser.cpp
        parser.h)
