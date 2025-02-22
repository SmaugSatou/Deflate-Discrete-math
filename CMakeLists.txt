cmake_minimum_required(VERSION 3.10)
project(Deflate)

set(CMAKE_CXX_STANDARD 17)

add_executable(deflate main.cpp huffman.cpp lz77.cpp filemanager.cpp)