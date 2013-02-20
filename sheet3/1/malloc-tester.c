#include <stdlib.h>
#include <unistd.h>

#define MAX_MEMORY 1073741824
#define FREE 0
#define BUSY 1
#define RAY_SIZE = 100

struct chunk
{ size_t ssz;
  int free;
  void* mem;
  struct chunk* next;
};

static struct chunk* start = NULL;
static void* end;

static void initiate()
{ start = sbrk(MAX_MEMORY);
  end = sbrk(0);
  start->ssz = MAX_MEMORY - sizeof(struct chunk);
  start->free = FREE;
  start->mem = start;
  start->next = start;
}

void* malloc(size_t input)
{ if(start==NULL) { initiate(); }
  struct chunk* newInit = start;

  struct chunk* incoming = { input, BUSY, newInit->mem, start};

  newInit->ssz = newInit->ssz - input;
  newInit->mem = newInit->mem + input;

  incoming->next = newInit;
  newInit->next = incoming;

  start = newInit;

  return incoming->mem;
}

void free(void* finger)
{ struct chunk* piece=start;

  while((piece=piece->next))
  { if(piece->mem == finger)
    { piece->free = FREE;
      break;
    }
    if(piece==start)
    {  break;
    }
  }
}
