#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_MEMORY 1024*1024*1024
#define SLACK 0
#define BUSY 1

struct CHUNK
{ size_t ssz;
  int busy;
  void* mem;
  struct CHUNK* next;
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
  freeBlock->next = NULL;

  new->ssz = size;
  new->busy = BUSY;
  new->mem = ((void *) new) + controlSize;
  new->next = freeBlock;

  return new;
}

static void initiate()
{ start = (chunk* ) sbrk(MAX_MEMORY);
  end = sbrk(0);

  start->ssz = MAX_MEMORY - controlSize;
  start->busy = SLACK;
  start->mem = ((void *) start) + controlSize;
  start->next = NULL;
}

void* mymalloc(size_t input)
{ if(start==NULL) { initiate(); }

  chunk* current = start;
  
  while(((current->busy != SLACK) && (current->ssz < input)) || (current->next != NULL))
  { current = current->next; }

  if(current->ssz < input) { return NULL; }
  else { return split(current, input); } //(current->busy == SLACK)
}

void myfree(void* finger)
{ if(finger<end && finger>((void *) start))
  { //pointer outside the range
    chunk* possible = ((chunk*) (((size_t) finger) - controlSize));
    if(possible->mem == finger)
    { //check that the pointer points to this chunk
      possible->busy = SLACK; 
    }
  }
  puts("free broke");
}

void printFrees()
{ chunk* piece = start;
  printf("First Free %p, size: %lu\n", (void *) piece, piece->ssz);
  while((piece=piece->next)!=NULL)
    printf("Free %p, size: %lu\n", (void *) piece, piece->ssz);
}
