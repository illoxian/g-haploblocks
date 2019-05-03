#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define _FILES_MAX_LENGTH 65536
char *_files_t1,_files_t2[_FILES_MAX_LENGTH];

#define ON_FAILURE_RETURN 0
#define ON_FAILURE_EXIT 1
#define FOPEN_READABLE(P,N,E) \
  if((P=fopen(N,"r")) == NULL) {\
    if((E)==ON_FAILURE_RETURN) {\
      fprintf(stderr,"Open file \"%s\" failed.\n",N);\
    }\
    else if((E)==ON_FAILURE_EXIT) {\
      fprintf(stderr,"Open file \"%s\" failed! Cannot proceed.\n",N);\
      exit(1);\
    }\
    else {\
      fprintf(stderr,"Unexpected exit status %d in FOPEN_READABLE."\
              " Cannot proceed.\n",E);\
      exit(1);\
    }\
  }
#define FOPEN_READABLE_BASE(P,N,B,E) \
  _files_t1 = (B);\
  if((P=fopen(N,"r")) == NULL) {\
    if((E)==ON_FAILURE_RETURN) {\
      if(_files_t1 == NULL) {\
        fprintf(stderr,"Open file \"%s\" failed.\n",N);\
      }\
      else {\
        sprintf(_files_t2,"%s/%s",_files_t1,N);\
        if((P=fopen(_files_t2,"r")) == NULL) {\
          fprintf(stderr,"Open file \"%s\" failed.\n",N);\
        }\
      }\
    }\
    else if((E)==ON_FAILURE_EXIT) {\
      if(_files_t1 == NULL) {\
        fprintf(stderr,"Open file \"%s\" failed! Cannot proceed.\n",N);\
        exit(1);\
      }\
      else {\
        sprintf(_files_t2,"%s/%s",_files_t1,N);\
        if((P=fopen(_files_t2,"r")) == NULL) {\
          fprintf(stderr,"Open file \"%s\" failed! Cannot proceed.\n",N);\
          exit(1);\
        }\
      }\
    }\
    else {\
      fprintf(stderr,"Unexpected exit status %d in FOPEN_READABLE_BASE."\
              " Cannot proceed.\n",E);\
      exit(1);\
    }\
  }
#define FOPEN_WRITEABLE(P,N,E) \
  if((P=fopen(N,"w")) == NULL) {\
    if((E)==ON_FAILURE_RETURN) {\
      fprintf(stderr,"Open file \"%s\" failed.\n",N);\
    }\
    else if((E)==ON_FAILURE_EXIT) {\
      fprintf(stderr,"Open file \"%s\" failed! Cannot proceed.\n",N);\
      exit(1);\
    }\
    else {\
      fprintf(stderr,"Unexpected exit status %d in FOPEN_WRITEABLE."\
              " Cannot proceed.\n",E);\
      exit(1);\
    }\
  }
#define FOPEN_APPENDABLE(P,N,E) \
  if((P=fopen(N,"a")) == NULL) {\
    if((E)==ON_FAILURE_RETURN) {\
      fprintf(stderr,"Open file \"%s\" failed.\n",N);\
    }\
    else if((E)==ON_FAILURE_EXIT) {\
      fprintf(stderr,"Open file \"%s\" failed! Cannot proceed.\n",N);\
      exit(1);\
    }\
    else {\
      fprintf(stderr,"Unexpected exit status %d in FOPEN_APPENDABLE."\
              " Cannot proceed.\n",E);\
      exit(1);\
    }\
  }

#define FCLOSE(P) fclose(P)

#define REMOVE(N) if(remove(N) != 0) {\
                    fprintf(stderr,"Can't remove file \"%s\".\n",N);\
                  }

#define FGETS(P,L,F) if(feof(F)) {\
                       fprintf(stderr,"fgets failed.\n");\
                     }\
                     else {\
                       fgets(P,L,F);\
                       P[strlen(P)-1] = '\0'; /* remove terminal newline */\
                     }

#define FSCANF(P,S,V) if(feof(P)) {\
                        fprintf(stderr,"fscanf failed.\n");\
                      }\
                      else {\
                        fscanf(P,S,V);\
                      }

#define OPEN_RDONLY(D,N) if((D=open(N,O_RDONLY)) == -1) {\
                           fprintf(stderr,"Can't open file \"%s\".\n",N);\
                         }

#define CLOSE(D) \
  if(close(D) != 0) {\
    fprintf(stderr,"close failed:(%s,%d)\n",__FILE__,__LINE__);\
    exit(1);\
  }

#define FSTAT(D,B) \
  if(fstat(D,B) == -1) {\
    fprintf(stderr,"\nfstat failed:(%s,%d)\n",__FILE__,__LINE__);\
    exit(1);\
  }

#define OPENDIR(H,D) \
  if((H=opendir(D)) == NULL) {\
    fprintf(stderr,"\nopendir failed:(%s,%d)\n",__FILE__,__LINE__);\
    exit(1);\
  }

#define CLOSEDIR(D) \
  if(closedir(D) != 0) {\
    fprintf(stderr,"Error closing file:(%s,%d)\n",__FILE__,__LINE__);\
    exit(1);\
  }

#endif /* FILES_H */
