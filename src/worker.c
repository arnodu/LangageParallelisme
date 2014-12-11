#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <mpi.h>

#include "bruteforce/bruteforce.h"

int main(int argc, char ** argv)
{
	assert(argc == 4);
	char* a = argv[1];
	int r = (int) *argv[2];
	char* m = argv[3];

  MPI_Init(NULL, NULL);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm master_comm;
  MPI_Comm_get_parent(&master_comm);

  /*fprintf(stderr,"%d : I'm worker #%d\n", getpid(), rank);
  fprintf(stderr,"  alphabet = %s\n",a);
	fprintf(stderr,"  max_size = %d\n",r);
  fprintf(stderr,"matching \"%s\"\n",m);*/

  while(1)
  {
		MPI_Send(NULL,0,MPI_INT,0,TAG_WORK_REQUEST,master_comm);
		sleep(1);
  }

  MPI_Finalize();
}
