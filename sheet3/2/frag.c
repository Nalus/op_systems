#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

#define MB 1024*1024

int main()
{ void* a = malloc(1000*MB);
  free(a);
  void* b = malloc(1022*MB);
  printf("%p\n",b);
  free(b);
}
