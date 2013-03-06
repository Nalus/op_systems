#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define N_THREADS 16
#define SIZE 1000
#define ITERATIONS 100000

#define MIN( a, b)  ((a)<(b)? (a): (b))

struct timespec start, finish;

typedef struct threadArgs threadArgs_t;

struct threadArgs {
  int tid;
  int from;
  int to;
};

double a[SIZE];
double b[SIZE];

pthread_barrier_t barrier;

/* Time difference between a and b in microseconds.  */
static int64_t
xelapsed (struct timespec a, struct timespec b)
{
    return ((int64_t)a.tv_sec - b.tv_sec) * 1000000
           + ((int64_t)a.tv_nsec - b.tv_nsec) / 1000LL;
}

void sequential( )
{ clock_gettime(CLOCK_REALTIME, &start);
  int i,j;
  for(j=0; j<ITERATIONS; j++)
  { for(i=0; i<SIZE; i++)
    {  a[i] = a[i] + b[i]; }
  }

  clock_gettime(CLOCK_REALTIME, &finish);
}




void * simd( void *arg)
{
  int tid,from,to;

  tid = ((threadArgs_t *)arg)->tid;
  from = ((threadArgs_t *)arg)->from;
  to = ((threadArgs_t *)arg)->to;
}

void initData( )
{
  int i;
  for( i=0; i<SIZE; i++) {
    a[i] = i;
    b[i] = (SIZE-1) - i;
  }
  pthread_barrier_init( &barrier, NULL, N_THREADS);
}

int main()
{ initData();sequential();

  fprintf (stderr, "Total time: %03li\n", xelapsed (finish, start));
  return(0);
}

  

