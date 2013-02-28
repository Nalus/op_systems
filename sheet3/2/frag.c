#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

#define MB 1024*1024

int main()
{ char** a = mymalloc(1000*MB);
  *a = "baa";

  myfree(a);

  void* b = mymalloc(1022*MB);
  if(b==NULL) printf("malloc failed, segmentation fault %p\n",b);
  else myfree(b);
}
