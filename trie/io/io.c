#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "files.h"
#include "alloc.h"
#include "io.h"

char *stream2string (FILE *fptr, unsigned int *n)
{
  static char *s;
  
  *n = 0;
  ALLOC(s,char,2);
  s[*n] = getc(fptr);
  
  while(s[*n]!=EOF && s[*n]!='\0' && s[*n]!='\n') {
    REALLOC(s,char,++*n+2);
    s[*n] = getc(fptr);
  }
  s[*n] = '\0';
  return(s);
} /* stream2string() */

char *file2string (char *name, unsigned int *n)
{
  int fd;
  struct stat buf;
  char *s=NULL;

  OPEN_RDONLY(fd,name);
  FSTAT(fd,&buf);
  *n = buf.st_size;
  if((s = mmap(s,*n,PROT_READ,MAP_SHARED,fd,0)) == NULL) {
    fprintf(stderr,"readfile: mmap with fd = %d and size = %d failed\n",
            fd,(int)buf.st_size);
    exit(1);
  } 
  CLOSE(fd);
  return(s);
} /* file2string() */

