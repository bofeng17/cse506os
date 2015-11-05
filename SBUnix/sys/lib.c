/*
 * lib.c
 *
 *  Created on: Nov 5, 2015
 *      Author: cao
 */
#include <stdlib.h>

void *
memset (void *s, int ch, size_t n)
{
  char* tmp = s;
  while (n > 0)
    {
      *tmp = ch;
      tmp++;
      n--;
    }
  return s;
}
