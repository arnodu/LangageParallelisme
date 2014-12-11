#include "common/work.h"
#include "common/mpi_tags.h"

int work_next(char* a, int r, struct work* w)
{
  //TODO
  w->begin++;
  w->end++;
  return 1;
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
