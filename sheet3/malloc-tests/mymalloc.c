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
  struct CHUNK* pred;
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
  freeBlock->ssz = new->ssz - (size + controlSize); //old size - new block size
  freeBlock->busy = SLACK;
  freeBlock->mem = ((void *) freeBlock) + controlSize;
  freeBlock->pred = new;
  freeBlock->succ = NULL;

  new->ssz = size;
  new->busy = BUSY;
  new->mem = ((void *) new) + controlSize;
  new->succ = freeBlock;
  //new->pred = new->pred; //stays the same, inherited from freeBlock

  return new;
}

static void initiate()
{ start = (chunk* ) sbrk(MAX_MEMORY);
  end = sbrk(0);

  start->ssz = MAX_MEMORY - controlSize;
  start->busy = SLACK;
  start->mem = ((void *) start) + controlSize;
  start->pred = NULL;
  start->succ = NULL;
}

void* mymalloc(size_t input)
{ if(start==NULL) { initiate(); }

  chunk* current = start;
  
  while(current->succ != NULL)
  { if((current->busy == SLACK) && (current->ssz > input))
    { return split(current,input)->mem; }

    current = current->succ;
  }

  //check the last element in the list
  if((current->busy == SLACK) && (current->ssz > input))
  { return split(current,input)->mem; }

  //if even the last element is not good enough, return NULL
  else { return NULL; }
}

//merge two free chunks
chunk* mergePair(chunk* older, chunk* younger)
{ older->ssz = older->ssz + younger->ssz + controlSize;
  older->succ = younger->succ;
  if(younger->succ != NULL)
  { younger->succ->pred = older; }
  //older->busy, older->pred and older-> mem are already set right

  return older;
}

void toMerge(chunk* piece)
{ //piece is already free, check its pred and succ for merger opportunities
  chunk* reborn = piece;

  if(reborn->succ != NULL)
  { if(reborn->succ->busy == SLACK)
    { reborn = mergePair(reborn, reborn->succ); }
  }

  if(reborn->pred != NULL)
  { if(reborn->pred->busy == SLACK)
    { reborn = mergePair(reborn->pred, reborn); }
  }
}

void myfree(void* finger)
{ if(finger<end && finger>=((void *) start))
  { //pointer outside the range
    chunk* possible = ((chunk *) (finger - controlSize));
    if(possible->mem == finger)
    { //check that the pointer points to this chunk
      possible->busy = SLACK; 
      toMerge(possible);
    }
    //else puts("not a real struct\n");
  }
  //else puts("out of bounds\n");
}

void printFrees()
{ chunk* piece = start;
  long unsigned freeMem = 0;

  if(piece->busy == SLACK)
  { printf("1st Free %p, size: %lu\n", (void *) piece, piece->ssz);
    freeMem += piece->ssz;
  }

  while((piece=piece->succ)!=NULL)
  { if(piece->busy == SLACK)
    { printf("Nth Free %p, size: %lu\n", (void *) piece, piece->ssz);
      freeMem += piece->ssz;
    }
  }

  printf("Total free memory: %lu; unfreed memory: %lu\n", freeMem, (MAX_MEMORY-(controlSize+freeMem)));
}

void* findMem(void* mem)
{ chunk* piece = start;
  while(piece!=NULL)
  { if(piece->mem == mem) { return ((void *) piece); }
    piece = piece->succ;
  }

  return NULL;
}
