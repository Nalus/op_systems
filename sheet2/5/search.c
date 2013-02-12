#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct input{char * file; char * pat;};
/* A function to searcg a substing in a file.  The file F has to be
   opened and the function is going to call subsequent fgetc on F
   until it reaches EOF or finds the match.

   Input arguments:
       F    -- input file opened for reading
       PAT  -- a pattern to be found.

   Return values:
       -1   -- in case the pattern was not found
      x > 0 -- where x is a position of the match.  */
int search (FILE *f, char *pat)
{ int i, j, c, pos = 1;
  const char *ptr;

  for (; EOF != (c = fgetc (f)); pos++)
  { if (c != *pat)
      continue;

    for (i = 1, ptr = pat + 1; *ptr != '\0'; i++, ptr++)
      if (*ptr != (c = fgetc (f))) {
        ungetc (c, f);
        break;
      }

      if (*ptr == '\0')
        return pos;
      else
        for (j = i - 1; j > 0; j--)
          ungetc (pat[j], f);
  }

  return -1;
}

void* search_wrapper(void* inst)
{ FILE * fp;
  int result;
  struct input* ins = (struct input*) inst;

  //open file stream, if file does not exist, return an error
  if((fp = fopen(ins->file, "r")) == NULL) { perror("Bad input\n"); }
  //search the file for the pattern
  result = search(fp, ins->pat);
  if (-1 == (result))
  //if pattern is not found
  { printf ("pattern '%s' was not found in %s\n", ins->pat, ins->file);
  } else
  //if found, print out the offset
  { printf ("pattern '%s' found in %s offset=%d\n", ins->pat,ins->file,result);
  }

  //flushe file stream 
  fclose(fp);
  //return nothing
}

int main (int argc, char *argv[])
{ if(argc<3)
  { puts("Please provide at least 2 arguements: a string pattern and a text file.\n");
    puts("For example: 'test' 1.txt\n");
    exit(EXIT_SUCCESS);
  }

  //allocate memory for array of pointers to threads and array of pointers to struct input
  pthread_t* tids = malloc ((argc-2)*sizeof(pthread_t));
  struct input** massive = malloc ((argc-2)*sizeof(struct input*));

  //allocate memory for each struct in array massive and set patten of each struct to sysem argument
  int i = 0;
  while(i<argc-2)
  { massive[i] = malloc(sizeof(struct input));
    //pattern is the msae for each file
    massive[i]-> pat = argv[1];
    i++;
  }

  //create thread for each file
  i=0;
  while(i<argc-2)
    //set file name to corresponding argument
  { massive[i]-> file = argv[i+2];
    //create thread and store its id in tids array
    pthread_create(&tids[i], NULL, search_wrapper, (void *) massive[i]);
    //printf("CREATE: %d\n", i);
    i++;
  }
  i=0;
  while(i<argc-2)
  { pthread_join(tids[i], NULL);
    //printf("JOIN: %d\n", i);
    i++;
  }

  //free all memory allocated for structures
  i=0;
  while(i<argc-2)
  { free(massive[i]);
    i++;
  }
  //free memory at array pointers
  free(massive); free(tids);
}
