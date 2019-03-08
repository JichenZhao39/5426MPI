//
// Created by JICHEN on 3/3/19.
//

#ifndef MPI_TMPI_RANK_H
#define MPI_TMPI_RANK_H

#include <mpi.h>
int TMPI_Rank(void *send_data,void *recv_data,MPI_Datatype datatype,MPI_Comm comm);

#endif //MPI_TMPI_RANK_H
