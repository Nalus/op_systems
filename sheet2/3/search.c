#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* A function to searcg a substing in a file.  The file F has to be
   opened and the function is going to call subsequent fgetc on F
   until it reaches EOF or finds the match.

   Input arguments:
       F    -- input file opened for reading
       PAT  -- a pattern to be found.

   Return values:
       -1   -- in case the pattern was not found
      x > 0 -- where x is a position of the match.  */
int search (FILE *f, const char *pat)
{ int i, j, c, pos = 1;
  const char *ptr;

  for (; EOF != (c = fgetc (f)); pos++)
  { if (c != *pat)
    continue;

    for (i = 1, ptr = pat + 1; *ptr != '\0'; i++, ptr++)
      if (*ptr != (c = fgetc (f)))
      { ungetc (c, f);
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

int main (int argc, char *argv[])
{ if(argc<3)
  { puts("Please provide at least 2 arguements: a string pattern and a text file.\n");
    puts("For example: #test 1.txt\n");
    exit(EXIT_SUCCESS);
  }

  //define variables
  int i = 2; //starts at 2 to skip the name of the binary and the pattern
  int pos;
  FILE * fp;
  //pattern is the same for every file
  const char* patt = argv[1];

  while(i<argc)
  //try to open the file, handle the error
  { if((fp = fopen(argv[i], "r")) == NULL) { perror("Bad input\n"); exit(EXIT_SUCCESS);}
    //search for the pattern in the file
    if (-1 == (pos=search(fp, patt)))
    //if not found
    { printf ("pattern '%s' was not found in %s\n", patt, argv[i]);
    } else
    //if found
    { printf ("pattern '%s' found in %s offset=%d\n", patt,argv[i],pos);
    }
    i++;
    //close file stream
    fclose(fp);
  }
  return EXIT_SUCCESS;
}
