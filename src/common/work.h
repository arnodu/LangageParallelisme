#ifndef WORK_H
#define WORK_H

#include <mpi.h>

struct work{
  int begin;
  int end;
};

int work_next(int a_size, int r, struct work* w);

void work_send(MPI_Comm workers, int rank, struct work* w);

void work_recv(MPI_Comm master, struct work* w);

#endif
