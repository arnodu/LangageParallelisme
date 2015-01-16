#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>

#include "util/mpi_tags.h"
#include "util/work.h"
#include "util/word.h"

#define WORK_GRANULARITY (WORK_SIZE/10 +1)

/***
 * Effectue la comparaison entre le mot actuel et le mdp
 ***/
static int compare_to_pwd(char* current, char* pwd, char* a)
{
  int equals = 1;
  int i;
  for(i=0; pwd[i]!='\0'; i++)
    equals &= (pwd[i] == a[(int)current[i]]);

  return equals && current[i]==0;

  /*for(int i=0; current[i]!=0; i++)
    printf("%d",current[i]);
  printf("\n");
  return 0;*/
}

/**
 * Traite le travail current_work sur nb_threads processeurs
 * @param current_work travail à effectuer
 * @param nb_threads nombre de threads à executer
 * @param out la chaine qui correspond au pwd (si le pwd a été trouvé)
 * @return vrai si le mpd a été trouvé, faux sinon
 */
static int do_work(const struct work* current_work, char* alpha, int max_size, char* pwd, int t, char* out)
{
  int alpha_size = strlen(alpha);

  int block = -1;
  int found = 0;
  #pragma omp parallel
  {
    int block_local;
    //block représente une file de blocs
    //pop un block
    #pragma omp atomic capture
      block_local = ++block;

    while(!found && WORK_GRANULARITY*block_local < current_work->size)
    {
      char local_str[max_size+1];
      if( word_add(local_str, current_work->begin, WORK_GRANULARITY*block_local, max_size, alpha_size) )
        break; //le mot du debut du bloc est trop long
      for(int i=0;  i<WORK_GRANULARITY && i+WORK_GRANULARITY*block_local < current_work->size && !found; i++)
      {
        if( compare_to_pwd(local_str, pwd, alpha) )
        { //pwd trouvé
          #pragma omp critical
          {
            found = 1;
            //Recopie du mot trouvé dans output
            int i;
            for(i=0; local_str[i] != 0; i++)
              out[i] = alpha[(int)local_str[i]];
            out[i] = '\0';
          }
          break;
        }
        //On a la derniere chaine possible: on termine
        if( word_inc(local_str, max_size, alpha_size) )
          break;
      }
      //pop un block
      #pragma omp atomic capture
        block_local = ++block;
    }
  }
  return found;
}

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

  char* s = malloc(r+1);

  int pwd_found = 0;
  struct work next_work;
  for(struct work current_work = WORK_NULL; !pwd_found ; current_work=next_work)
  {
    //Envoi d'une requête de travail, preparation à recevoir la réponse
    MPI_Request requests[2];
    MPI_Isend(NULL, 0, MPI_INT, 0, TAG_WORK_REQUEST, comm_master, &requests[0]);
    MPI_Irecv(&next_work, 1, MPI_Type_work, 0, TAG_SEND_WORK, comm_master, &requests[1]);

    //Faire le travail
    pwd_found = do_work(&current_work, a, r, m, t, s);

    //Attente du travail suivant
    MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    if(next_work.size == 0)
      break;//Plus de travail
  }

  //Envoi (ou non) de la réponse
  if(pwd_found)
    MPI_Send(s, r, MPI_CHAR, 0, TAG_PWD_FOUND, comm_master);
  else
    MPI_Send(NULL, 0, MPI_INT, 0, TAG_WORK_DONE, comm_master);

  free(s);

  MPI_Comm_free(&comm_master);
  MPI_Finalize();
}


