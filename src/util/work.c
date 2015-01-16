#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "util/work.h"
#include "util/word.h"
#include "util/mpi_tags.h"

#define MIN(a,b) ((a)>(b))?(b):(a)

struct work WORK_NULL = {{0},0};

void print_work(struct work* work)
{
  printf("{%d, ",work->size);
  for(int i=0; work->begin[i]!=0; i++)
     printf("%d ", work->begin[i]);
  printf("}\n");
}

int work_next(int alpha_size, int r, struct work* work)
{

  //work->begin += WORK_SIZE
  int rem = word_add(work->begin, work->begin, WORK_SIZE, r, alpha_size);

  //if(work->begin > "zzzzz"){work->begin = "zzzzz"; work->size=0}
  if(rem != 0)
  {
    for(int i=0; i<r; i++)
      work->begin[i] = alpha_size;
    work->size = 0;
    return 0;
  }

  //begin + WORK_SIZE can be > "zzzzzz"
  work->size = WORK_SIZE;

  return work->size;
}

MPI_Datatype MPI_Type_create_work()
{
  MPI_Datatype MPI_Type_begin;
  MPI_Type_contiguous(WORK_MAX_WORD_SIZE, MPI_CHAR, &MPI_Type_begin);

  struct work* w = NULL;
  int bl[] = {1, 1};
  MPI_Aint disp[] = {(MPI_Aint)&(w->begin), (MPI_Aint)&(w->size)};
  MPI_Datatype types[] = {MPI_Type_begin, MPI_INT};

  MPI_Datatype MPI_Type_work;
  MPI_Type_create_struct(2, bl, disp, types, &MPI_Type_work);
  MPI_Type_commit(&MPI_Type_work);

  return MPI_Type_work;
}
