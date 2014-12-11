#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include <omp.h>

#include "common/mpi_tags.h"
#include "common/work.h"

static void manage_work();
static void do_work(int tid);

int main(int argc, char ** argv)
{
  //Get program arguments
  assert(argc == 5);
  int t = (int)*argv[1];
  char* a = argv[2];
  int r = (int) *argv[3];
  char* m = argv[4];


  //Get MPI infos
  int provided;
  MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);
  assert(provided == MPI_THREAD_FUNNELED);

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
#include <unistd.h>
static void manage_work()
{
  MPI_Comm comm_master;
  MPI_Comm_get_parent(&comm_master);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  while(1)
  {
    MPI_Send(NULL,0,MPI_INT,0,TAG_WORK_REQUEST,comm_master);

    struct work work;
    work_recv(comm_master, &work);

    printf("Work received: [%d, %d].. pretending to work\n",work.begin, work.end);

    sleep(1);
  }
}

static void do_work(int tid)
{
  //TODO
}
