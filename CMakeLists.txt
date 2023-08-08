cmake_minimum_required(VERSION 3.23)
project(stream)

set(CMAKE_CXX_STANDARD 20)

add_library(stream STATIC src/serializer.cpp src/codec.cpp)

include_directories(include)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(test)