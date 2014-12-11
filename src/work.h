#ifndef WORK_H
#define WORK_H

#include <mpi.h>

struct work{
  int begin;
  int end;
};

MPI_Datatype get_work_type();

#endif
