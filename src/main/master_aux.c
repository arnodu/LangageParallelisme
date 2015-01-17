#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "util/mpi_tags.h"
#include "util/work.h"

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
    WORK_SIZE *= p*t;

    struct work work = WORK_NULL;
    int active_workers = p-1; //les workers spawnés sont actifs
    int found = 0;//le mdp n'a pas encore été trouvé
    //Tant qu'on a des workers actifs
    while(active_workers > 0)
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
            MPI_Recv(s, r+1, MPI_CHAR, status.MPI_SOURCE, TAG_PWD_FOUND, comm_workers, MPI_STATUS_IGNORE);
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
