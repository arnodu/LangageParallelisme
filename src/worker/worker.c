#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include <omp.h>

#include "common/mpi_tags.h"
#include "common/work.h"

/**
 * Send a work request to master and get a new work in return
 * @param comm_master communicator where master has rank 0
 * @param w modified to contain the new work from master
 * @return true if master sent work, false otherwise
 */
static int request_work(MPI_Comm comm_master, struct work* w);

/**
 * Do the computation on t threads
 * If do_work is called inside a parallel region, omp_set_nested must be 1 to have multithreading
 * @param current_work work to achieve
 * @param t number of threads to use
 * @return true if the pwd has been found, false otherwise
 */
static int do_work(const struct work* current_work, int t);

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
	MPI_Comm comm_master;
	MPI_Comm_get_parent(&comm_master);
	omp_set_nested(1);//Set MPI_NESTED for do_work (if not set the for is not parallelized)

	int pwd_found = 0;
	int has_work = 1;
	struct work next_work;
	for(struct work current_work = {0,0}; !pwd_found && has_work; current_work=next_work)
	{
		//omp parallel inside loop for synchro
		#pragma omp parallel num_threads(2)
		{
			if(omp_get_thread_num() == 0)
				has_work = request_work(comm_master, &next_work);
			else
				pwd_found = do_work(&current_work, t);
		}
	}

	MPI_Finalize();
}

static int request_work(MPI_Comm comm_master, struct work* next_work)
{
	MPI_Send(NULL,0,MPI_INT,0,TAG_WORK_REQUEST,comm_master);
	work_recv(comm_master, next_work);

	//TODO: detecter quand il n'y a plus de travail

	return 1;//Always get work
}

static int do_work(const struct work* current_work, int t)
{
	#pragma omp parallel for num_threads(t)
	for(int i = current_work->begin; i<=current_work->end; i++)
	{
		//TODO : comparaison de chaines
		printf("%d\t[%d,%d]\n",i, current_work->begin, current_work->end);
	}

	return 0;//Never find pwd
}
