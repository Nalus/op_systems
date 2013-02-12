#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <pthread.h>
#include <unistd.h>


/* Dummy function spawned by every thread/process.  */
void* dummy ()//void *data)
{ return NULL;
}

/* Time difference between a and b in microseconds.  */
int64_t xelapsed (struct timespec a, struct timespec b)
{
    return ((int64_t)a.tv_sec - b.tv_sec) * 1000000
           + ((int64_t)a.tv_nsec - b.tv_nsec) / 1000LL;
}

/* Measure the time for NUMBER fork creations.  */
void measure_forks (unsigned number)
{ struct timespec start, stop, finish;
  unsigned i = 0;

  clock_gettime(CLOCK_REALTIME, &start);
  pid_t* pids = malloc (number*sizeof(pid_t));
  for (i = 0;i < number; i++)
  { pids[i] = fork();
    if(pids[i] == 0)
    { dummy(NULL);
      exit(EXIT_SUCCESS);
    }
  }
  clock_gettime(CLOCK_REALTIME, &stop);

  for (i = 0; i < number; i++)
  { wait(&pids[i]);
  }
  clock_gettime(CLOCK_REALTIME, &finish);

  printf ("process: num=%03u, fork=%03li, wait=%03li, total=%03li\n",
          number, xelapsed (stop, start), xelapsed (finish, stop),
          xelapsed (finish, start));
}

/* Measure the time for NUMBER thread creations.  */
void measure_threads (unsigned number)
{ struct timespec start, stop, finish;
  unsigned i = 0;

  clock_gettime(CLOCK_REALTIME, &start);
  int threadSuc;
  pthread_t* tids = malloc (number*sizeof(pthread_t));
  for (i = 0;i < number; i++)
  { if((threadSuc = pthread_create(&tids[i], NULL, dummy, NULL)) != 0) { perror("Bad thread id, exiting.\n"); exit(EXIT_SUCCESS);}
  }
  clock_gettime(CLOCK_REALTIME, &stop);

  for (i = 0; i < number; i++)
  { pthread_join(tids[i], NULL);
  }
  clock_gettime(CLOCK_REALTIME, &finish);

  printf ("thread: num=%03u, fork=%03li, join=%03li, total=%03li\n",
          number, xelapsed (stop, start), xelapsed (finish, stop),
          xelapsed (finish, start));
}

int main (int argc, char *argv[])
{ int load =0;
  if(argc !=2)
  { printf("Provide the number of instaces to run as an integer.\n");
    exit(EXIT_SUCCESS);
  }
  if((load = atoi(argv[1]))<0)
  { printf("Please provide only positive integers as arguements.\n");
    exit(EXIT_SUCCESS);
  }

  measure_forks (load);
  measure_threads (load);
  return EXIT_SUCCESS;
}
