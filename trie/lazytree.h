/*****************************************************************************
 *  FILE   : lazytree.h
 *  AUTHOR : Jens Stoye
 *  DATE   : 06-09/2018
 *****************************************************************************/
#ifndef LAZYTREE_H
#define LAZYTREE_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "types.h"
#include "binarymatrix.h"

/*****************************************************************************
 * TYPE DEFINITIONS
 *****************************************************************************/
typedef struct LazyNode lazynode;
struct LazyNode {
  lazynode *parent,*left,*right; /* left/right used only in evaluated nodes */
  Ushrt w,num;
};

/*****************************************************************************
 * FUNCTION DECLARATIONS
 *****************************************************************************/
void outputsubmatrix(FILE *fptr,lazynode *n,Uint offset,Uint d,Uint t,bm *M);
void add(lazynode *root,Ushrt w,bm *M);
void findmax(FILE *fptr,lazynode *root,Uint t,bm *M);
lazynode *advance(FILE *fptr,lazynode *root,Uint offset,Uint t,bm *M);
void listleaves(FILE *fptr,lazynode *n,Uint d,bm *M);
void printtree(FILE *fptr,lazynode *n,Uint d,bm *M);
void freenode(lazynode *n);

#endif /* LAZYTREE_H */
/****** EOF (binarymatrix.h) *************************************************/
