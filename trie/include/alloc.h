#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>

#define ALLOC(P,T,S) if(((P)=(T*)malloc(sizeof(T)*(S))) == NULL) {\
          fprintf(stderr,"malloc failed (%s:%d)\n",__FILE__,__LINE__);\
          exit(1);\
        }
#define REALLOC(P,T,S) if(((P)=(T*)realloc(P,sizeof(T)*(S))) == NULL) {\
          fprintf(stderr,"realloc failed (%s:%d)\n",__FILE__,__LINE__);\
          exit(1);\
        }
#define FREE(P) free(P)

#endif /* ALLOC_H */
