cmake_minimum_required(VERSION 3.12)
project(MPI C)

set(CMAKE_C_STANDARD 99)

add_executable(MPI main.c tmpi_rank.c tmpi_rank.h )
add_executable(chapter Chapter3/mpiex.c)
add_executable(parallel MPIprogram/MPI_Parallel.c MPIprogram/MPI_Parallel.h)
add_executable(ass Assignment/assignment1.c)
add_executable(ass_mpi  Assignment/Assignment_MPI.c)
add_executable(threads PThreads/Pthread_ex.c )
add_executable(tutorial MPIprogram/tutorial.c)
add_executable(tutorial3 MPIprogram/tutorial3_1.c)
add_executable(tutorial32 MPIprogram/tutorial3_2.c)
add_executable(linuxex PThreads/Linux_Unix.c PThreads/lib/tlpi_hdr.h PThreads/lib/get_num.h PThreads/lib/error_functions.h PThreads/lib/error_functions.c PThreads/lib)

include_directories(/usr/local/include/)