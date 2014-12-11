#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#define TAG_WORK_REQUEST 35
#define TAG_FOUND 66

/**
 * Essaie toutes les chaines possibles de tailles <r sur l'alphabet a et tente de les matcher avec m
 * L'algorithme est distribué sur p processus avec t threads par processus
 * /!\ MPI doit etre initialisé avec MPI_Init avant
 * @param m : mot de passe à trouver
 * @param p : nombre de processus (p-1 esclaves)
 * @param t : nombre de threads/processus
 * @param a : alphabet
 * @param r : taille max du mdp
 * @param s : mot de passe en sortie
 * @return booleen indiquant si le mdp a été trouvé
 **/
int bruteforce(int p, int t ,char* a ,int r ,char* m , char* s);

#endif
