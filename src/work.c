#include "work.h"

MPI_Datatype get_work_type()
{
  struct work* w = NULL;
  int bl[] = {sizeof(int), sizeof(int)};
  void* disp[] = {&(w->begin), &(w->end)};
  MPI_Datatype types[] = {MPI_INT, MPI_INT};
  MPI_Datatype MPI_Type_work;
  MPI_Type_create_struct(2, bl, (MPI_Aint*)disp, types, &MPI_Type_work);
  MPI_Type_commit(&MPI_Type_work);

  return MPI_Type_work;
}
