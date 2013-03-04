#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

#define KB 1024
#define N 10

int main()
{ void* array[10];
  int i = 0;
  while(i<N)
  { array[i] = mymalloc(KB);
    i++;
  }

  i=0;
  while(i<N)
  { myfree(array[i]);
    i++;
  }
  printFrees();
}
