#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "master_aux.c"

#define SIZE_MAX 5

char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
char pwd[SIZE_MAX+1];

static void bench(int nb_workers, int nb_threads, int i)
{
  //Ouverture du fichier bench_mdp_<nb_workers>x<nb_threads> en ecriture
  char filename[20];
  snprintf(filename, 20, "bench_mdp_%dx%d", nb_workers, nb_threads);
  FILE* output_file = fopen(filename, "a");

  //for(int i=1; i<=SIZE_MAX; i++)
  {
    printf("%d\n",i);

    for(int j=0; j<i; j++)
      pwd[j] = 'z';
    pwd[i] = '\0';

    char res_pwd[SIZE_MAX] = {0};

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    //--------------------------------
    //Debut mesure

    int res = master(nb_workers+1, nb_threads ,alphabet ,i ,pwd ,res_pwd);

    //--------------------------------
    //Fin mesure
    gettimeofday(&tv2, NULL);

    //Ecriture de la mesure
    double msec = (double)(tv2.tv_sec - tv1.tv_sec)*1000.0+ (double)(tv2.tv_usec - tv1.tv_usec)/1000.0;
    fprintf(output_file, "%d \t %f \n", i, msec);

    assert(res);
    for(int j=0; j<i; j++)
      assert(res_pwd[j] == 'z');
    assert(res_pwd[i] == '\0');
  }

  fclose(output_file);
}

int main(int argc, char** argv)
{
  assert(argc == 4);
  int nb_proc = atoi(argv[1]);
  int nb_thread = atoi(argv[2]);
  int i = atoi(argv[3]);


  MPI_Init(NULL, NULL);

  bench(nb_proc, nb_thread, i);

  MPI_Finalize();
}
