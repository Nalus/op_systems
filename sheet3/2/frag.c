#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

#define MB 1024*1024

int main()
{ void* a = mymalloc(1000*MB);

  myfree(a);

  void* b = mymalloc(1022*MB);
  printf("Busy %p\n",b);

  myfree(b);
}
