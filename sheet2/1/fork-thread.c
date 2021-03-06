#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <unistd.h>


/* Dummy function spawned by every thread/process.  */
void* dummy()
//made this function return NULL to satisfy the pthread_create method
{ return NULL;
}

/* Time difference between a and b in microseconds.  */
int64_t xelapsed (struct timespec a, struct timespec b)
{ return ((int64_t)a.tv_sec - b.tv_sec) * 1000000 + ((int64_t)a.tv_nsec - b.tv_nsec) / 1000LL;
}

/* Measure the time for NUMBER fork creations.  */
void measure_forks (unsigned number)
{ struct timespec start, stop, finish;
  unsigned i = 0;

  //store start time, handle error on failure
  if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("Bad time.\n"); exit(EXIT_SUCCESS);}
  pid_t* pids = malloc (number*sizeof(pid_t));
  for (i = 0;i < number; i++)
  { pids[i] = fork();
    //in the child
    if(pids[i] == 0)
    { dummy();
      //free the pointer memory that has been forked
      free(pids);
      exit(EXIT_SUCCESS);
    }
    else if(pids[i] < 0)
    { perror("Bad fork.\n"); exit(EXIT_SUCCESS);}
  }
  //store the stop time
  if(clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("Bad time.\n"); exit(EXIT_SUCCESS);}

  for (i = 0; i < number; i++)
  //wait for every child for die
  { wait(&pids[i]);
  }
  //get the time needed for every child to die
  if(clock_gettime(CLOCK_REALTIME, &finish) == -1) { perror("Bad time.\n"); exit(EXIT_SUCCESS);}

  //print out the time
  printf ("process: num=%03u, fork=%03li, wait=%03li, total=%03li\n",
          number, xelapsed (stop, start), xelapsed (finish, stop),
          xelapsed (finish, start));
  //free memory used for array of process ids
  free(pids);
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
  return EXIT_SUCCESS;
}
