#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "common/work.h"
#include "common/mpi_tags.h"

#define WORK_SIZE 10000

#define MIN(a,b) ((a)>(b))?(b):(a)

int work_next(int alpha_size, int r, struct work* work)
{
	//Update max_work_int = r^alpha_size
	static int max_work_int, old_alpha_size, old_r;
	if(alpha_size != old_alpha_size || r != old_r || max_work_int == 0)
	{
		old_alpha_size = alpha_size;
		old_r = r;
		float max_work = powf(alpha_size+1,r+1);
		assert(max_work < INT_MAX);
		max_work_int = (int) max_work;
	}

	//Sanity check
	assert(work->end <= max_work_int);
	assert(work->begin <= work->end);

	work->begin = MIN(max_work_int, work->end+1);
	work->end = MIN(max_work_int, work->begin+WORK_SIZE);

  return work->end - work->begin;
}

static MPI_Datatype MPI_Type_work = NULL;

static MPI_Datatype set_type_work()
{
  struct work* w = NULL;
  int bl[] = {1, 1};
  MPI_Aint disp[] = {(MPI_Aint)&(w->begin), (MPI_Aint)&(w->end)};
  MPI_Datatype types[] = {MPI_INT, MPI_INT};
  MPI_Datatype MPI_Type_work;
  MPI_Type_create_struct(2, bl, disp, types, &MPI_Type_work);
  MPI_Type_commit(&MPI_Type_work);

  return MPI_Type_work;
}

void work_send(MPI_Comm comm_workers, int rank, struct work* w)
{
  if( MPI_Type_work == NULL)
    MPI_Type_work = set_type_work();
  MPI_Send(w, 1, MPI_Type_work, rank, TAG_SEND_WORK, comm_workers);
}

void work_recv(MPI_Comm comm_master, struct work* w)
{
  if( MPI_Type_work == NULL)
    MPI_Type_work = set_type_work();
  MPI_Recv(w, 1, MPI_Type_work, 0, TAG_SEND_WORK, comm_master, MPI_STATUS_IGNORE);
}
