#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "util/mpi_tags.h"
#include "util/work.h"

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
    char s[r+1]; s[0] = '\0';

    MPI_Init(&argc, &argv);

    int res = master(p, t ,a ,r ,m ,s);

    MPI_Finalize();

    //Affiche le résultat
    if(res == 1)
        printf("Found : %s\n",s);
    else
        printf("Not found!\n");
}

static int master(int p, int t ,char* a ,int r ,char* m , char* s)
{
		int a_size = strlen(a);

    //Spawn les workers en leur donant t et r en paramètres
    MPI_Comm comm_workers;
    char tc[] = {(char)t,'\0'};
    char rc[] = {(char)r,'\0'};
    char *argv[] = {tc, a, rc, m, NULL};
    MPI_Comm_spawn("./worker", argv, p-1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &comm_workers, MPI_ERRCODES_IGNORE);

    MPI_Datatype MPI_Type_work = MPI_Type_create_work(r);

    struct work work = WORK_NULL;
    int active_workers = p-1; //les workers spawnés sont actifs
    int found = 0;//le mdp n'a pas encore été trouvé
    //Tant qu'on a des workers actifs
    while(active_workers > 0 && !found)
    {
        //On scrute les messages entrants
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm_workers, &status);

        if(status.MPI_TAG == TAG_WORK_REQUEST)
        {
            MPI_Recv(NULL,0,MPI_INT,status.MPI_SOURCE,TAG_WORK_REQUEST,comm_workers,MPI_STATUS_IGNORE);
            //Répondre à la requete
            if(!found)
							work_next(a_size, r, &work);//Déterminer le travail suivant
						else//Si le mdp a été trouvé, on envoie un work vide pour que le worker s'arrete
							work = WORK_NULL;

            //Envoi du travail
						MPI_Send(&work, 1, MPI_Type_work, status.MPI_SOURCE, TAG_SEND_WORK, comm_workers);
        }
        else if(status.MPI_TAG == TAG_PWD_FOUND)
        {//mdp trouvé : recevoir le resultat
            //Receive result in s
            MPI_Recv(s, r, MPI_CHAR, status.MPI_SOURCE, TAG_PWD_FOUND, comm_workers, MPI_STATUS_IGNORE);
            found = 1;//Le master ne donnera plus de travail
            active_workers --;
        }
        else if(status.MPI_TAG == TAG_WORK_DONE)
        {//Le worker à recu un travail vide et s'est bien arrêté
            MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, TAG_WORK_DONE, comm_workers, MPI_STATUS_IGNORE);
            active_workers --;
        }
        else
					assert(0);
    }

    MPI_Comm_free(&comm_workers);

    return found;
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
        *p = 2;
        *t = 2;
        *a = malloc(27);
        for(int i = 0; i<26; i++)
            (*a)[i] = i+'a';
        (*a)[26] = '\0';
        *r = 4;
    }
    else if(argc == 6)
    {
        *m = argv[1];
        *p = atoi_checked(argv[2]);
        *t = atoi_checked(argv[3]);
        *a = argv[4];
        *r = atoi_checked(argv[5]);

        if(*p<=0 || *t<=0 || *r<=0)
        {
            fprintf(stderr, "Tous les paramètres doivent etre positifs!\n");
            exit(EXIT_FAILURE);
        }
        if(*r > WORK_MAX_WORD_SIZE)
        {
            fprintf(stderr, "Le nombre max de lettre est trop grand (>%d)!\n",WORK_MAX_WORD_SIZE);
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
