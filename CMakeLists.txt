cmake_minimum_required(VERSION 3.12)
project(MPI C)

set(CMAKE_C_STANDARD 99)

add_executable(MPI main.c tmpi_rank.c tmpi_rank.h )
add_executable(chapter Chapter3/mpiex.c)
add_executable(parallel MPIprogram/MPI_Parallel.c MPIprogram/MPI_Parallel.h)
add_executable(ass Assignment/assignment1.c)
include_directories(/usr/local/include/)