cmake_minimum_required(VERSION 3.27)
project(mywebserver)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(includes)

add_executable(mywebserver
        includes/Cluster.hpp
        includes/Parser.hpp
        includes/utils.hpp
        includes/webserver.hpp
        srcs/Cluster.cpp
        srcs/Parser.cpp
        srcs/utils.cpp
        srcs/webserver.cpp
        junk.cpp)
