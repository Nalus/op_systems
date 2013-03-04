/***********************************
 * program by Konstantin Devyatov
 * the memory management is done through 2 linked lists: one tracks the whole memory, the other - only slack (free) chunk
 * 
 * malloc calls are handled by looping through the slackList and finding the memory chunk big enough for the input
 * this approach supports reasonaly space efficiency by tucking the biggest chunk of memory at the end of both lists
 *
 * the free calls are handled by freeing the chunk that holds the given pointer (with safety checks), then inserting that into the slackList
 *
 * Possible improvements:
 *   making the slackList into a tree to increase the time efficiency of loops
 *   increasing the space efficiency by looping through the whole slackList and using the chunk closest of size closest to requested
 *   implementing thread safety
 *   
 *   finding and fixing the current bugs
 *     the program takes longer than expected on malloc-tester-03 (which it is not supposed to handle)
 *     the program segfaults on malloc-tester-04 for unknown reason
***********************************/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_MEMORY 1024*1024*1024
#define SLACK 0
#define BUSY 1

//control structure to support memory management
struct CHUNK
{ void* mem;
  size_t ssz;
  int busy;
  struct CHUNK* pred;
  struct CHUNK* succ;
};
typedef struct CHUNK chunk;

static chunk* start = NULL;
static chunk* slackStart = NULL;
static chunk* lastBusy = NULL;
static void* end = NULL;
static const size_t controlSize = sizeof(chunk);

//splits a free chunks to accomodate allocation
//the allocation happens at the start of the free block
chunk* splitSlack (chunk* slackBlock, size_t size)
{ chunk* new = slackBlock;
  slackBlock = (chunk*)((size_t)new + size + controlSize); //old location + size of (control structure + data)

  if(new==slackStart) { slackStart = slackBlock; } //if splitting slackStart, refresh the pointer

  //reshape the last(slack) block with new parameters 
  slackBlock->ssz = new->ssz - (size + controlSize); //old size - new block size
  slackBlock->busy = SLACK;
  slackBlock->mem = (void*)((size_t) slackBlock + controlSize);
  slackBlock->pred = new->pred;
  slackBlock->succ = new->succ;

  new->ssz = size;
  new->busy = BUSY;
  new->mem = (void*)((size_t)new + controlSize);
  new->succ = slackBlock;
  if(new==start) { new->pred = NULL; }
  else { new->pred = lastBusy; } //stay the same, inherited from 

  lastBusy = new;

  return new;
}

chunk* findSlack(size_t size)
{ chunk* current = slackStart;

  while(current->succ !=NULL)
  { if(current->ssz >= size)
    { return current; }

    current=current->succ;
  }

  //check the last element in the list
  if(current->ssz >= size)
  { return current; }

  //if even the last element is not good enough, return NULL
  else { return NULL; }
}

static void initiate()
{ start = (chunk* )sbrk(MAX_MEMORY);
  slackStart = start;
  end = sbrk(0);

  slackStart->ssz = MAX_MEMORY - controlSize;
  slackStart->busy = SLACK;
  slackStart->mem = (void*)((size_t) start + controlSize);
  slackStart->pred = NULL;
  slackStart->succ = NULL;
}

void* mymalloc(size_t input)
{ if(start==NULL) { initiate(); }

  chunk* available = findSlack(input);

  if(available == NULL) { return NULL; }
  else { return splitSlack(available, input)->mem; } //splitSlack needs to return chunk*
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

void insertIntoSlack(chunk* piece)
{ int trippleMerged = 0;

  //merge with predecessor
  if(piece->pred != NULL)
  { if(piece->pred->busy == SLACK)
    { piece=mergePair(piece->pred, piece);
     trippleMerged = 1;
    }
  }

  //merge with successor
  //if two merges happen; no more needs to be done
  if(piece->succ != NULL)
  { if(piece->succ->busy == SLACK)
    { if(piece->succ == slackStart) { slackStart = piece; } //make sure to refresh startSlack pointer
      piece=mergePair(piece, piece->succ);
      if(trippleMerged == 1) { return; }
    }
  }

  //if piece is before the first slack node (in the memory), make piece the new slackStart; no more needs to be done
  if(piece < slackStart)
  { piece->succ = slackStart;
    piece->pred = NULL;
    slackStart->pred = piece;
    slackStart = piece;
    return;
  }

  chunk* currentSlack = slackStart;
  while(currentSlack->succ < piece && currentSlack->succ != NULL)
  { currentSlack=currentSlack->succ; }

  if(currentSlack->succ == NULL) { return; } //i am the last element , the list ends with a free node (98% of times)

  piece->succ = currentSlack->succ;
  currentSlack->succ->pred = piece;
  piece->pred = currentSlack;
  currentSlack->succ = piece;
}

void myfree(void* finger)
{ if(finger<end && finger>= (void *)start)
  { //pointer outside the range
    chunk* possible = ((chunk *) ((size_t)finger - controlSize));
    if(possible->mem == finger)
    { //check that the pointer points to this chunk
      possible->busy = SLACK; 
      insertIntoSlack(possible);
    }
    //else puts("not a real struct\n");
  }
  //else puts("out of bounds\n");
}

void printFrees()
{ chunk* piece = slackStart;
  long unsigned slackMem = 0;

  if(slackStart == NULL) { printf("No slack blocks in the memory"); }
  else
  {
    //redundant condition, slackList only contains slack chunks
    /*if(piece->busy == SLACK)
    {*/ printf("1st Free %p, size: %lu\n", (void *)piece, piece->ssz);
      slackMem += piece->ssz;
    /*}*/

    while((piece=piece->succ)!=NULL)
    { /*if(piece->busy == SLACK)
      {*/ printf("Nth Free %p, size: %lu\n", (void *)piece, piece->ssz);
        slackMem += piece->ssz;
      /*}*/
    }

    //unfreed memory ignores one control structure
    printf("Total slack memory: %lu; unfreed memory: %lu\n", slackMem, (MAX_MEMORY-(controlSize+slackMem)));
  }
}

void* findMem(void* mem)
{ chunk* piece = start;
  while(piece!=NULL)
  { if(piece->mem == mem) { return (void *)piece; }
    piece = piece->succ;
  }

  return NULL;
}
