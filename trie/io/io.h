#ifndef IO_H
#define IO_H

#include <stdio.h>

char *stream2string (FILE *fptr, unsigned int *n);
char *file2string (char *name, unsigned int *n);

#endif /* IO_H */
