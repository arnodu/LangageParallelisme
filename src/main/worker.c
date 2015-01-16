#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>

#include "util/mpi_tags.h"
#include "util/work.h"
#include "util/word.h"

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

	MPI_Datatype MPI_Type_work = MPI_Type_create_work();

	omp_set_num_threads(t);

	int pwd_found = 0;
	char* s = malloc(r+1);
	struct work next_work;
	for(struct work current_work = WORK_NULL; !pwd_found ; current_work=next_work)
	{
    MPI_Request requests[2];
    MPI_Isend(NULL, 0, MPI_INT, 0, TAG_WORK_REQUEST, comm_master, &requests[0]);
    MPI_Irecv(&next_work, 1, MPI_Type_work, 0, TAG_SEND_WORK, comm_master, &requests[1]);

    pwd_found = do_work(&current_work, a, r, m, t, s);

    MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    if(next_work.size == 0)
      break;
	}

	if(pwd_found)
		MPI_Send(s, r, MPI_CHAR, 0, TAG_PWD_FOUND, comm_master);
	else
		MPI_Send(NULL, 0, MPI_INT, 0, TAG_WORK_DONE, comm_master);

  free(s);

	MPI_Comm_free(&comm_master);
	MPI_Finalize();
}

static int compare_to_pwd(char* current, char* pwd, char* a)
{
  int equals = 1;
  for(int i=0; pwd[i]!='\0'; i++)
    equals &= (pwd[i] == a[(int)current[i]]);

  return equals;
}

#define WORK_GRANULARITY WORK_SIZE/10 +1

static int do_work(const struct work* current_work, char* a, int r, char* m, int t, char* s)
{
  int alpha_size = strlen(a);

  int block = -1;
	int found = 0;
	#pragma omp parallel
	{
    int block_local;
    #pragma omp atomic capture
        block_local = ++block;

    while(!found && WORK_GRANULARITY*block_local < current_work->size)
    {
      char local_str[r+1];
      if( word_add(local_str, current_work->begin, WORK_GRANULARITY*block_local, r, alpha_size) )
        break; //word has more than r letters

      for(int i=0;  i<WORK_GRANULARITY
                &&  i+WORK_GRANULARITY*block_local < current_work->size
                &&  !found; i++)
      {
        if( compare_to_pwd(local_str, m, a) )//found pwd
        {
          #pragma omp critical
          {
            found = 1;
            int i;
            for(i=0; local_str[i] != 0; i++)
              s[i] = a[(int)local_str[i]];
            s[i] = '\0';
          }
          break;
        }

        //On a la derniere chaine: on termine
        if( word_inc(local_str, r, alpha_size) )
          break;
      }
      #pragma omp atomic capture
          block_local = ++block;
    }
  }
	return found;
}
