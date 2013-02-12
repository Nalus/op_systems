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
{
    int i, j, c, pos = 1;
    const char *ptr;

    for (; EOF != (c = fgetc (f)); pos++) {
        if (c != *pat)
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
  int * result = malloc(sizeof(int));
  struct input* ins = (struct input*) inst;
  if((fp = fopen(ins->file, "r")) == NULL) { perror("Bad input\n"); }
  *result = search(fp, ins->pat);
  if (-1 == (*result))
  { printf ("pattern '%s' was not found in %s\n", ins->pat, ins->file);
  } else
  { printf ("pattern '%s' found in %s offset=%d\n", ins->pat,ins->file,*result);
  }
  fclose(fp);
  free(result);// free(ins);// free(inst);
  pthread_exit(NULL);
}

int main (int argc, char *argv[])
{ if(argc<3)
  { puts("Please provide at least 2 arguements: a string pattern and a text file.\n");
    puts("For example: #test 1.txt\n");
    exit(EXIT_SUCCESS);
  }
  
  pthread_t* tids = malloc ((argc-2)*sizeof(pthread_t));
  struct input* massive[argc];

  int i = 0;
  while(i<argc)
  { massive[i] = malloc(sizeof(struct input));
    massive[i]-> pat = argv[1];
    i++;
  }

  i=2;
  while(i<argc)
  { massive[i-2]-> file = argv[i];
    pthread_create(&tids[i-2], NULL, search_wrapper, (void *) massive[i-2]);
    //printf("CREATE: %d\n", i);
    i++;
  }
  i=2;
  while(i<argc)
  { pthread_join(tids[i-2], NULL);
    //printf("JOIN: %d\n", i);
    i++;
  }

  //free all memory allocated for structures
  i=0;
  while(i<argc)
  { free(massive[i]);
    i++;
  }
  free(tids);
}
