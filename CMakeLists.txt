cmake_minimum_required(VERSION 3.10)
project(RISC_V_Simulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(code
        instruction.hpp
        main.cpp
        stage.cpp
        variate.cpp)
