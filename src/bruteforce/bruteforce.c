#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <mpi.h>

#include <bruteforce/bruteforce.h>

struct work{
	char* begin;
	int nb;
}

#define WORK_SIZE 1000


// work_begin += work_size
// work_size mis à jour si il n'y a plus assez de chaines possibles
//retourne le nombre de chaines à calculer
int next_work(char* a, int r, struct work * work)
{
	//TODO
	return WORK_SIZE;
}

int bruteforce(int p, int t ,char* a ,int r ,char* m , char* s)
{
	assert(p>0);
	assert(t>0);
	assert(a!=NULL);
	assert(r>0);
	assert(m!=NULL);
	assert(s!=NULL);

	MPI_Comm workers_comm;
	char rc[] = {(char)r,'\0'};
	char *argv[] = {a, rc, m, NULL};
	MPI_Comm_spawn("./worker", argv, p-1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &workers_comm, MPI_ERRCODES_IGNORE);

	MPI_Request recv_work_request, recv_found;
	MPI_Recv_init(NULL,0,MPI_INT,MPI_ANY_SOURCE,TAG_WORK_REQUEST,workers_comm,&recv_work_request);
	MPI_Recv_init(s,   r,MPI_INT,MPI_ANY_SOURCE,TAG_FOUND,       workers_comm,&recv_found);

	MPI_Request requests[] = {recv_work_request, recv_found};
	MPI_Startall(2,requests);



	char work_begin[r+1]; work_begin[0] = '\0';
	int work_size = 10000;
	int ended = 0;
	while(ended<p-1)
	{
		int id;
		MPI_Status status;
		MPI_Waitany(2,requests,&id,&status);
		if(status.MPI_TAG == TAG_WORK_REQUEST)
		{
			char begin[r+1];
			int work_size = next_work(a, r, work_begin);
			if(work_size > 0)
			{
				send_work(status.MPI_SOURCE, work_begin, work_size);
			}
			else
			{
				stop_worker(status.MPI_SOURCE);
				if(ended++ == p-1)
					return 0;
			}
		}
		else if(status.MPI_TAG == TAG_FOUND)
		{
			for(int i=0; i<p-1; i++)
			{
				stop_worker(i);
			}
			return 1;
		}
		else
			assert(0);
	}
}
