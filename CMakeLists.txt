cmake_minimum_required (VERSION 3.0)
project (Cs112)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic -std=c++17")
include_directories(include)

add_executable(SquarePlant src/main.cpp src/box.cpp src/plant.cpp)
