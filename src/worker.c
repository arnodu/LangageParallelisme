#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <omp.h>

#include <mpi.h>

#include "bruteforce/bruteforce.h"
#include "work.h"

static MPI_Comm master_comm;

void manage_work()
{
  MPI_Send(NULL,0,MPI_INT,0,TAG_WORK_REQUEST,master_comm);
  
  struct work work;
  MPI_Datatype MPI_Type_work = get_work_type();
  MPI_Recv(&work, 1, MPI_Type_work, 0, TAG_SEND_WORK, master_comm, MPI_STATUS_IGNORE); 

  printf("Work received: [%d, %d]\n",work.begin, work.end);
}

void do_work(int tid)
{
  //TODO
}

int main(int argc, char ** argv)
{
  assert(argc == 4);
  int t = (int)*argv[1];
  char* a = argv[2];
  int r = (int) *argv[3];
  char* m = argv[4];
  int provided;
  MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  
  MPI_Comm_get_parent(&master_comm);

  /*fprintf(stderr,"%d : I'm worker #%d\n", getpid(), rank);
    fprintf(stderr,"  alphabet = %s\n",a);
    fprintf(stderr,"  max_size = %d\n",r);
    fprintf(stderr,"matching \"%s\"\n",m);*/

#pragma omp parallel num_threads(t+1)
  {
    int tid = omp_get_thread_num();
    if(tid == 0)
      manage_work();
    else
      do_work(tid);
  }

  MPI_Finalize();
}
