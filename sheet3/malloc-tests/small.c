#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

#define KB 1024
#define MB 1024*1024
#define N 10

int main()
{ void* array[10];
  int i = 0;
  while(i<=N/2)
  { array[i] = mymalloc(KB);
    array[i+1] = malloc(100*MB);
    printf("malloc took %p and %p\n",array[i],array[i+1]);
    i+=2;
  }

  i=0;
  while(i<N)
  { myfree(array[i]);
    printf("free took %p\n",array[i]);
    i+=2;
  }
  i=1;
  while(i<N)
  { myfree(array[i]);
    printf("free took %p\n",array[i]);
    i+=2;
  }
  
  printFrees();
}
