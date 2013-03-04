#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_MEMORY 1024*1024*1024
#define FREE 0
#define BUSY 1

struct CHUNK
{ size_t ssz;
  int free;
  void* mem;
  struct CHUNK* next;
};
typedef struct CHUNK chunk;

static void* start = NULL;
static void* end = NULL;
const size_t controlSize = sizeof(chunk);

//splits a free chunks to accomodate allocation
//the allocation happens at the start of the free block
chunk* split (chunk* freeBlock, size_t size)
{ chunk* new = freeBlock;
  freeBlock = ((chunk*) (((void *) new) + size + controlSize)); //old location + size of (control structure + data)

  //reshape the last(free) block with new parameters 
  freeBlock->ssz = new->ssz - (size + controlSize);
  freeBlock->free = FREE;
  freeBlock->mem = ((void *) freeBlock) + controlSize;
  freeBlock->next = NULL;

  new->ssz = size;
  new->free = BUSY;
  new->mem = ((void *) new) + controlSize;
  new->next = freeBlock;

  return new;
}

static void initiate()
{ start = sbrk(MAX_MEMORY);
  end = sbrk(0);

  chunk* free = ((chunk*) start);

  free->ssz = MAX_MEMORY - controlSize;
  free->free = FREE;
  free->mem = ((void *) free) + controlSize;
  free->next = NULL;
}

void* mymalloc(size_t input)
{ if(start==NULL) { initiate(); }

  chunk* current = ((chunk*) start);
  
  while(((current->free != FREE) && (current->ssz < input)) || (current->next != NULL))
  { current = current->next; }

  if(current->ssz < input) { return NULL; }
  else { return split(current, input); } //(current->free == FREE)
}

void myfree(void* finger)
{ if(finger<end && finger>start)
  { //pointer outside the range
    chunk* possible = ((chunk*) (((size_t) finger) - controlSize));
    if(possible->mem == finger)
    { //pointer not to a real chunk
      possible->free = FREE; //free the pointer
    }
  }
}
