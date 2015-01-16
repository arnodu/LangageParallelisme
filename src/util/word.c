/*** word.c
 * Implémentation des fonctions de word.h
 **/

#include "word.h"

int word_inc(char* str, int size_max, int alpha_size)
{
  return word_add(str, str, 1, size_max, alpha_size);
}

int word_add(char* dest,const char* str, int to_add, int size_max, int alpha_size)
{
  int i;
  for(i=0; i<size_max && to_add != 0; i++)
  {
    int current_letter=str[i];

    if(current_letter == 0)
    {
      dest[i+1] = 0;
      current_letter++;
      to_add--;
    }

    current_letter += to_add%alpha_size;
    int retenue = (current_letter-1)/alpha_size;
    current_letter -= retenue*alpha_size;

    dest[i] = current_letter;

    to_add = (to_add/alpha_size) + retenue;
  }

  return to_add;
}
