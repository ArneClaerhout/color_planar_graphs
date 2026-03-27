cmake_minimum_required(VERSION 3.22)
project(v5_C C)

find_package(OpenMP REQUIRED)


set(CMAKE_C_STANDARD 11)



add_executable(v5_C
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

target_link_libraries(v5_C PUBLIC OpenMP::OpenMP_C)
