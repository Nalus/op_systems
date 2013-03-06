#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SIZE 1000
#define ITERATIONS 300000

struct timespec start, finish;

int a[SIZE];
int b[SIZE];

/* Time difference between a and b in microseconds.  */
static int64_t
xelapsed (struct timespec a, struct timespec b)
{
    return ((int64_t)a.tv_sec - b.tv_sec) * 1000000
           + ((int64_t)a.tv_nsec - b.tv_nsec) / 1000LL;
}

void initData()
{ int i;
  for( i=0; i<SIZE; i++)
  { a[i] = i;
    b[i] = (SIZE-1) - i;
  }
}

void add_arrays()
{ int i;
  for(i=0; i<SIZE; i++)
  {  a[i] = a[i] + b[i]; }
}

void main(int argc, char** argv)
{  initData();

   clock_gettime(CLOCK_REALTIME, &start);
   int i;
   for(i=0; i < ITERATIONS; i++)
   { add_arrays(); }
   clock_gettime(CLOCK_REALTIME, &finish);

  printf("%i\n", a[500]);
  fprintf (stderr, "Total time: %03li\n", xelapsed (finish, start));
}
