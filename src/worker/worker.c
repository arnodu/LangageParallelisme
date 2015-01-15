#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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
static int do_work(const struct work* current_work, char* a, int r, char* m, int t, char* s);

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
	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
	assert(provided == MPI_THREAD_FUNNELED);
	MPI_Comm comm_master;
	MPI_Comm_get_parent(&comm_master);
	omp_set_nested(1);//Set MPI_NESTED for do_work (if not set the for is not parallelized)

	int pwd_found = 0;
	int has_work = 1;
	char* s = malloc(r+1);
	struct work next_work;
	#pragma omp parallel
	for(struct work current_work = {0,0}; !pwd_found && has_work; current_work=next_work)
	{
			#pragma omp task
				has_work = request_work(comm_master, &next_work);

			#pragma omp task
				pwd_found = do_work(&current_work, a, r, m, t, s);

      #pragma omp taskwait
	}

	if(pwd_found)
	{
		MPI_Send(s, r, MPI_CHAR, 0, TAG_PWD_FOUND, comm_master);
	}
	else
		MPI_Send(NULL, 0, MPI_CHAR, 0, TAG_WORK_DONE, comm_master);

  free(s);

	MPI_Comm_free(&comm_master);
	MPI_Finalize();
}

static int request_work(MPI_Comm comm_master, struct work* next_work)
{
	MPI_Send(NULL,0,MPI_INT,0,TAG_WORK_REQUEST,comm_master);
	work_recv(comm_master, next_work);

	return next_work->end != next_work->begin;
}

static int do_work(const struct work* current_work, char* a, int r, char* m, int t, char* s)
{
	int a_count = strlen(a);

	int found = 0;
	#pragma omp parallel for num_threads(2) schedule(dynamic)
	for(int i = current_work->begin; i<=current_work->end; i++)
	{
		char c[r+1]; c[r+1] = '\0';
		int current = i;
		for(int k=0; k<r; k++)
		{
			if(current%(a_count+1) == 0)
				c[k] = '\0';
			else
				c[k] = a[current%(a_count+1)-1];

			current = current/(a_count+1);
		}

		if(!strcmp(c,m))
		{
			found = 1;
			#pragma omp critical
			strcpy(s,c);
		}
	}
	return found;
}










		/*printf("Doing %d : ",i);
		int current = i, k=0;
		while(current!=0)
		{
			printf("%c",current%(a_count+1) == 0 ? '_' : a[current%(a_count+1)]);
			if(current%(a_count+1) == 0)
				if(m[k] == '\0')
				{
					current = 0;
					break;
				}
				else break;
			else if(a[current%(a_count+1)-1] != m[k])
				break;
			else
				current/=a_count;

			k++;
		}
		printf("\n");

		if(current==0 && m[k] == '\0')
			found = i;
	}

	if(found>0)
	{
		int k=0;
		while(found!=0)
		{
			s[k] = a[found%a_count];
			found/=a_count;
			k++;
		}
		s[k]='\0';
		return 1;
	}
	else
		return 0;
}
*/
