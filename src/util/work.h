/** --- work.h ---
  * Contient la gestion du travail pour le master.
 **/

#ifndef WORK_H
#define WORK_H

#include <mpi.h>

#define WORK_MAX_WORD_SIZE 20
#define WORK_SIZE_DEFAULT 100000

int WORK_SIZE;

///Structure decrivant un travail
struct work{
  char begin[WORK_MAX_WORD_SIZE]; //Chaine de début
  int size; //nombre de chaines à tester
};

///Travail vide
struct work WORK_NULL;

/**
 * Cree le type de données mpi pour envoyer/recevoir du travail
 **/
MPI_Datatype MPI_Type_create_work();

/**
 * Récupère le travail qui succède à w
 * begin ne peut pas avoir plus de lettres que size_max,
 * le dernier travail commencera par le dernier mot possible et sera de taille 0
 * la taille du travail dépend de WORK_SIZE
 * @param a_size taille de l'alphabet
 * @param size_max taille max du mdp
 * @param w (entree) le travail précédent (sortie) travail suivant
 * @return la taille du travail retourné
 **/
int work_next(int a_size, int size_max, struct work* w);

#endif
