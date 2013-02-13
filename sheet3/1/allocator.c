#include <stdlib.h>
#include <unistd.h>

struct guard
{ size_t ssz;
  int free;
  void* mem;
}

void* allocate(size_t input)
{ struct guard acc = 
  if(sbrk(sizeof(input)) !=
}
