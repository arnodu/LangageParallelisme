#ifndef WORK_H
#define WORK_H

#include <mpi.h>

#define WORK_MAX_WORD_SIZE 10
#define WORK_SIZE 10000

struct work{
  char begin[WORK_MAX_WORD_SIZE];
  int size;
};

struct work WORK_NULL;

MPI_Datatype MPI_Type_create_work();

void work_init(int max_word_size);

int work_next(int a_size, int r, struct work* w);

#endif
