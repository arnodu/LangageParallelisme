/*** word.h
 * Contient la gestion des chaines de caractères pour itérer sur l'ensemble des mdp possibles
 ***/

#ifndef WORD_H
#define WORD_H

/**
 * Ajoute to_add a str et le stocke dans dest.
 * Si le résultat possède plus de size_max caractères, la fonction retourne une erreur
 * Attention : les elements sont dans [0, alpha_size], 0 le delimiteur de fin de chaine
 * Exemples: pour alpha_size = 9
 *    {0}   + 1 -> {1,0}
 *    {1,0} + 2 -> {3,0}
 *    {9,0} + 1 -> {1,1,0}
 *    ...
 * @param dest (output) resultat de l'addition
 * @param str (input) chaine a additionner
 * @param to_add entier a ajouter
 * @param size_max taille max du mot : erreur si le résultat dépasse
 * @param alpha_size taille de l'alphabet
 **/
int word_add(char* dest, const char* str, int to_add, int size_max, int alpha_size);

/**
 * Similaire à word_add, mais sur place et avec to_add = 1
 **/
int word_inc(char* str, int size_max, int alpha_size);

#endif
