#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define N_THREADS 31
#define SIZE 1000
#define ITERATIONS 100000

#define MIN( a, b)  ((a)<(b)? (a): (b))

struct timespec sstart, sfinish, pstart, pfinish;

struct threadArgs {
  pthread_t tid;
  int from;
  int to;
};
typedef struct threadArgs threadArgs_t;

double a[SIZE];
double b[SIZE];
threadArgs_t** arg;

//pthread_barrier_t barrier;

/* Time difference between a and b in microseconds.  */
static int64_t
xelapsed (struct timespec a, struct timespec b)
{
    return ((int64_t)a.tv_sec - b.tv_sec) * 1000000
           + ((int64_t)a.tv_nsec - b.tv_nsec) / 1000LL;
}

void sequential( )
{ int i,j;
  for(j=0; j<ITERATIONS; j++)
  { for(i=0; i<SIZE; i++)
    {  a[i] = a[i] + b[i]; }
  }
}

void * simd( void *arg)
{ int tid,from,to,i;

  tid = ((threadArgs_t *)arg)->tid;
  from = ((threadArgs_t *)arg)->from;
  to = ((threadArgs_t *)arg)->to;
//  printf("TID: %i: from %i and to %i\n",(int)tid, from, to);

  for(i=from; i<to; i++)
  {  a[i] = a[i] + b[i]; }

  return NULL;
}

void initData()
{
  int i;
  for( i=0; i<SIZE; i++) {
    a[i] = i;
    b[i] = (SIZE-1) - i;
  }
  //pthread_barrier_init( &barrier, NULL, N_THREADS);
}

//this function splits SIZE by number as equally as possible
//and then returns an array of positions of chunk endings (to, including the last element)
void splitting(int number)
{ int rest,common,even_size,i;

  rest = SIZE % number;
  even_size = SIZE - rest;
  common = even_size / number;

  arg[0]->from = 0;
  arg[0]->to = common;
  if(rest > 0) { arg[0]->to = arg[0]->to + rest; }

  for(i=1; i<number; i++)
  { arg[i]->from = arg[i-1]->to;
    arg[i]->to = arg[i]->from + common;
  }
}

void parallel(int thread_n)
{ int k,l;
  arg = malloc(sizeof(threadArgs_t*)*thread_n);
  //allocate memory to every struct
  for(l=0; l<thread_n; l++) { arg[l] = malloc(sizeof(threadArgs_t)); }

  splitting(thread_n);

  for(k=0; k<ITERATIONS; k++)
  { for(l=0; l<thread_n; l++)
    { if(pthread_create(&arg[l]->tid,NULL,simd,arg[l]) != 0) { perror("Thread not created\n"); exit(EXIT_SUCCESS);} }

    for(l=0; l<thread_n; l++)
    { if(pthread_join(arg[l]->tid, NULL) != 0) { perror("Thread not created\n"); exit(EXIT_SUCCESS);} }
  }

  for(l=0; l<thread_n; l++) { free(arg[l]); }
  free(arg);
}

int main(int argc, char** argv)
{ initData();
  if(argc != 2) { printf("Please provide one number < 30, to specify number of threads\n"); exit(EXIT_SUCCESS); }
  int thread_n = atoi(argv[1]);
  if(thread_n == 0 || thread_n > 30) { printf("Please make sure your nubmer is a valid integer <30\n"); exit(EXIT_SUCCESS); }

  //parallel implementation
  clock_gettime(CLOCK_REALTIME, &pstart);
  parallel(thread_n);
  clock_gettime(CLOCK_REALTIME, &pfinish);

  //sequential implementation
  clock_gettime(CLOCK_REALTIME, &sstart);
  sequential();
  clock_gettime(CLOCK_REALTIME, &sfinish);

  fprintf (stderr, "Total parallel time: %03li\n", xelapsed (pfinish, pstart));
  fprintf (stderr, "Total sequential time: %03li\n", xelapsed (sfinish, sstart));

  return(EXIT_SUCCESS);
}
