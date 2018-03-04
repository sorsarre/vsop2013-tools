cmake_minimum_required(VERSION 3.7)
project(vsop2013-tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libvsop2013)
add_subdirectory(vsop-convert)
