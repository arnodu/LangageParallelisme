#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#include "common/mpi_tags.h"
#include "common/work.h"

static void parse_arguments(int argc, char** argv, int* p, int* t, char** a, int* r, char** m);
static int master(int p, int t ,char* a ,int r ,char* m , char* s);

int main(int argc, char ** argv)
{
    //--- Entrées ---//
    int p;//Nombre de processeurs (p-1 esclaves)
    int t;//Threads par escalve
    char* a;//Alphabet de recherche
    int r;//Taille maximum du mdp
    char *m;//mdp à découvrir

    parse_arguments(argc, argv, &p, &t, &a, &r, &m);

    //--- Sortie ---
    char s[r+1];

    MPI_Init(NULL,NULL);

    /*fprintf(stderr,"%d : I'm master\n", getpid());
      fprintf(stderr,"bruteforce (%dx%d threads) :\n", p, t);
      fprintf(stderr,"  alphabet = %s\n",a);
      fprintf(stderr,"  max_size = %d\n",r);
      fprintf(stderr,"matching \"%s\"\n",m);*/

    int res = master(p, t ,a ,r ,m ,s);

    MPI_Finalize();

    //Show result
    if(res == 1)
        printf("Found : %s\n",s);
    else
        printf("Not found!\n");
}

static int master(int p, int t ,char* a ,int r ,char* m , char* s)
{
    //Spawn the workers
    MPI_Comm comm_workers;
    char tc[] = {(char)t,'\0'};
    char rc[] = {(char)r,'\0'};
    char *argv[] = {tc, a, rc, m, NULL};
    MPI_Comm_spawn("./worker", argv, p-1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &comm_workers, MPI_ERRCODES_IGNORE);

    //Initiate connexion with workers : TAG_WORK_REQUEST, TAG_PWD_FOUND
    MPI_Request recv_work_request;
    MPI_Recv_init(NULL,0,MPI_INT,MPI_ANY_SOURCE,TAG_WORK_REQUEST,comm_workers,&recv_work_request);

    //While there is an active worker
    struct work work = {0,1};
    int active_workers = p-1;
    while(active_workers > 0)
    {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm_workers, &status);
        //Message received from worker
        if(status.MPI_TAG == TAG_WORK_REQUEST)
        {
            MPI_Start(&recv_work_request);
            //Answer work request
            int work_size = work_next(a, r, &work); //get work
            if(work_size > 0) //If valid work, send it
            {
                work_send(comm_workers, status.MPI_SOURCE, &work);
            }
            else //If no work left
            {
                //stop_worker(status.MPI_SOURCE);
                active_workers--;
            }
            MPI_Wait(&recv_work_request, MPI_STATUS_IGNORE);
        }
        else if(status.MPI_TAG == TAG_PWD_FOUND) //If found return
        {
            //Receive result in s
            MPI_Recv(s, r, MPI_INT, MPI_ANY_SOURCE, TAG_PWD_FOUND, comm_workers, MPI_STATUS_IGNORE);
            //stop_workers();
            return 1;
        }
    }
    return 0;
}

/** Parse a string into an unsigned int
 *  @return the parsed integer; -1 if negative or cannot be parsed
 **/
static int atoi_checked(char* s)
{
    char* endptr;
    long int l = strtol(s, &endptr, 10);
    if(*endptr != '\0')
    {
        fprintf(stderr, "WARNING : string \"%s\" cannot be parsed to a positive integer\n", s);
        return -1;
    }
    else if(l <= 0 || l > INT_MAX)
    {
        return -1;
    }
    else
        return (int) l;
}

static void parse_arguments(int argc, char** argv, int* p, int* t, char** a, int* r, char** m)
{
    if(argc == 2)
    {
        *m = argv[1];
        *p = 4;
        *t = 2;
        *a = malloc(27);
        for(int i = 0; i<26; i++)
            (*a)[i] = i+'a';
        (*a)[26] = '\0';
        *r = 4;
        *m = argv[1];
    }
    else if(argc == 6)
    {
        *m = argv[1];
        *p = atoi_checked(argv[2]);
        *t = atoi_checked(argv[3]);
        *a = argv[4];
        *r = atoi_checked(argv[5]);

        if(p<=0 || t<=0 || r<=0)
        {
            fprintf(stderr, "Tous les paramètres doivent etre positifs!\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("./master m [p t a r]\n");
        printf("  m : mot de passe à trouver\n");
        printf("  p : nombre de processus (p-1 esclaves)\n");
        printf("  t : nombre de threads/processus\n");
        printf("  a : alphabet\n");
        printf("  r : taille max du mdp\n");
        exit(EXIT_FAILURE);
    }
}
