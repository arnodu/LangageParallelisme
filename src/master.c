#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

#include <bruteforce/bruteforce.h>

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
  else if(l <= 0 || l > INT_MAX){
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

  int res = bruteforce(p, t ,a ,r ,m ,s);

  MPI_Finalize();

  if(res == 1)
  {
    printf("Found : %s\n",s);
    assert(!strcmp(m,s));
  }
  else
  {
    printf("Not found!\n");
  }
}
