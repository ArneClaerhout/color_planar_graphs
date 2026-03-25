cmake_minimum_required(VERSION 3.22)
project(v5_C_copy C)

set(CMAKE_C_STANDARD 11)

add_executable(v5_C_copy
        graphs/colorings.c
        graphs/colorings.h
        graphs/vertex.c
        graphs/vertex.h
        graphs/main.c
        graphs/graph.c
        graphs/graph.h
        graphs/main.h
        graphs/counter.c
        graphs/counter.h
        graphs/types.h)
