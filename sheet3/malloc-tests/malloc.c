#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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

static void* start = NULL;
static void* end = NULL;
static struct chunk* first = NULL;
static size_t controlSize = sizeof(struct chunk);

static void initiate()
{ start = sbrk(MAX_MEMORY);
  end = sbrk(0);

  first = (struct chunk*) start;
  first->ssz = MAX_MEMORY - controlSize;
  first->free = FREE;
  first->mem = (void *) ((size_t) start + controlSize);
  first->next = NULL;
}

void* malloc(size_t input)
{ if(start==NULL) { initiate(); }

  struct chunk* incoming;
  struct chunk* current = (struct chunk*) start;

  while((current != NULL) && (current->free != FREE) && (current->ssz < input))
  { current = current->next;
  }

  if(current==NULL) { return NULL; }
  else //(current->free == FREE)
  { incoming = current;

    incoming->ssz = input;
    incoming->free = BUSY;
    incoming->mem = (void *) ((size_t) incoming + controlSize);

    current = (struct chunk*) ((size_t) incoming->mem + incoming->ssz);
    current->ssz = current->ssz - (input + controlSize);
    current->mem = (void *) ((size_t) current + controlSize);

    incoming->next = current;

    return incoming->mem;
  }
}

void free(void* finger)
{ struct chunk* piece=first;

  while(piece != NULL)
  { if(piece->mem == finger)
    { piece->free = FREE;
    }

    piece = piece->next;
  }

  /*if(first->mem == finger)
  { first=successor; 
  }

  struct chunk* predecessor=first;

  while(piece != NULL)
  { printf("inside while\n");
    successor = piece->next;
    if(piece->mem == finger)
    { if(piece == first)
      { first = NULL;
        break;
      }
      else
      { predecessor->next = successor;
        break;
      }
    }

    predecessor = piece;
    piece=piece->next;
  }*/
}
