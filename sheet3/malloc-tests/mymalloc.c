#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_MEMORY 1024*1024*1024
#define SLACK 0
#define BUSY 1

struct CHUNK
{ void* mem;
  size_t ssz;
  int busy;
  struct CHUNK* succ;
};
typedef struct CHUNK chunk;

static chunk* start = NULL;
static void* end = NULL;
const static size_t controlSize = sizeof(chunk);

//splits a free chunks to accomodate allocation
//the allocation happens at the start of the free block
chunk* split (chunk* freeBlock, size_t size)
{ chunk* new = freeBlock;
  freeBlock = ((chunk*) (((void *) new) + size + controlSize)); //old location + size of (control structure + data)

  //reshape the last(slack) block with new parameters 
  freeBlock->ssz = new->ssz - (size + controlSize);
  freeBlock->busy = SLACK;
  freeBlock->mem = ((void *) freeBlock) + controlSize;
  freeBlock->succ = NULL;

  new->ssz = size;
  new->busy = BUSY;
  new->mem = ((void *) new) + controlSize;
  new->succ = freeBlock;

  return new;
}

static void initiate()
{ start = (chunk* ) sbrk(MAX_MEMORY);
  end = sbrk(0);

  start->ssz = MAX_MEMORY - controlSize;
  start->busy = SLACK;
  start->mem = ((void *) start) + controlSize;
  start->succ = NULL;
}

void* mymalloc(size_t input)
{ if(start==NULL) { initiate(); }

  chunk* current = start;
  
  while(((current->busy != SLACK) && (current->ssz < input)) || (current->succ != NULL))
  { current = current->succ; }

  if(current->ssz < input) { return NULL; }
  else { return split(current, input)->mem; } //(current->busy == SLACK)
}

chunk* mergePair(chunk* prev, chunk* next)
{ prev->ssz = prev->ssz + next->ssz;
  prev->succ = next->succ;

  return prev;
}

void merge(chunk* piece)
{ //need doubly-linked list implementation
}

void myfree(void* finger)
{ if(finger<end && finger>=((void *) start))
  { //pointer outside the range
    chunk* possible = ((chunk *) (finger - controlSize));
    if(possible->mem == finger)
    { //check that the pointer points to this chunk
      possible->busy = SLACK; 
    }
    //else puts("not a real struct\n");
  }
  //else puts("out of bounds\n");
}

void printFrees()
{ chunk* piece = start;
  printf("1st Free %p, size: %lu\n", (void *) piece, piece->ssz);
  while((piece=piece->succ)!=NULL)
    printf("Nth Free %p, size: %lu\n", (void *) piece, piece->ssz);
}

void* findMem(void* mem)
{ chunk* piece = start;
  while(piece!=NULL)
  { if(piece->mem == mem) { return ((void *) piece); }
    piece = piece->succ;
  }

  return NULL;
}
