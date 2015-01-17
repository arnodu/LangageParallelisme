#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "util/word.h"

static void test_word_add()
{
  char str[10] = {0};
  char res[10];
  int rem;

  for(int i=0; i<220; i++)
  {
    int rem = word_add(res, str, i*1000, 8, 9);
    printf("%d:\t",i);
    for(int k=0; res[k]!=0; k++)
      printf("%d",res[k]);
    printf("\trem=%d\n",rem);
  }

  for(int i=0; i<10; i++)
    str[i] = 0;

  //alphabet = "123456789"

  //"" + 1 = "1"
  rem = word_add(res, str, 1, 2, 9);
  assert(rem == 0);
  assert(res[0] == 1);
  assert(res[1] == 0);

  //"" + 10 = "9"
  rem = word_add(res, str, 9, 2, 9);
  assert(rem == 0);
  assert(res[0] == 9);
  assert(res[1] == 0);

  //"" + 12 = "21"
  rem = word_add(res, str, 10, 2, 9);
  assert(rem == 0);
  assert(res[0] == 1);
  assert(res[1] == 1);
  assert(res[2] == 0);

  //"" + 100 = "121" trucated
  rem = word_add(res, str, 100, 2, 9);
  assert(res[0] == 1);
  assert(res[1] == 2);
  assert(rem == 1);
  assert(res[2] == 0);

  //"9" + 1 = "11"
  str[0] = 9; str[1] = 0;
  rem = word_add(res, str, 1, 2, 9);
  assert(rem == 0);
  assert(res[0] == 1);
  assert(res[1] == 1);
  assert(res[2] == 0);

  //"11" + 1 = "21"
  str[0] = 1; str[1] = 1; str[2] = 0;
  rem = word_add(res, str, 1, 2, 9);
  assert(rem == 0);
  assert(res[0] == 2);
  assert(res[1] == 1);
  assert(res[2] == 0);

  //"22"(#20) + 10 = "33"(#30)
  str[0] = 2; str[1] = 2; str[2] = 0;
  rem = word_add(res, str, 10, 2, 9);
  assert(rem == 0);
  assert(res[0] == 3);
  assert(res[1] == 3);
  assert(res[2] == 0);

  str[0] = 9; str[1] = 9; str[2] = 0;
  rem = word_add(res, str, 1, 2, 9);
  assert(rem != 0);

  str[0] = 1; str[1] = 1; str[2] = 0;
  rem = word_add(res, str, 0, 2, 9);
  assert(rem == 0);
  assert(res[0] == 1);
  assert(res[1] == 1);
  assert(res[2] == 0);

  str[0] = 9; str[1] = 9; str[2] = 0;
  rem = word_add(res, str, 1, 2, 9);
  assert(rem == 1);
}

int main()
{
  test_word_add();
}
