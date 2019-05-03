/*****************************************************************************
 *  FILE   : haploblocks.c
 *  AUTHOR : Jens Stoye
 *  DATE   : 06-09/2018
 *****************************************************************************
 *
 *****************************************************************************/

/*****************************************************************************
 * SWITCHES
 *****************************************************************************/
//#define LOG
//#define DEBUG

/*****************************************************************************
 * CONSTANTS
 *****************************************************************************/
#define ROW_ALLOCSTEP 1048576

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h> //exit()
#include <string.h> //strlen()

#include "binarymatrix.h"
#include "lazytree.h"

#include "types.h"
#include "files.h"
#include "alloc.h"
#include "io.h"

/*****************************************************************************
 * GLOBAl FUNCTION MAIN
 *****************************************************************************/
int main(int argc,char *argv[]) {

  FILE *fptr;
  Ushrt i;
  Uint j,t;
  binarymatrixstatus status;
  bm *M;
  lazynode *root;

//check argument and open file    
  if(argc<2 || argc>3 || (argc==3 && atoi(argv[2])<0)) {
    fprintf(stderr,"Usage: %s <matrix_filename> [<minimal_block_size>]\n",argv[0]);
    return(EXIT_FAILURE);
  }
  FOPEN_READABLE(fptr,argv[1],ON_FAILURE_EXIT);

  t = argc==3 ? atoi(argv[2]) : 2;

#ifdef LOG
  fprintf(stderr,"Reading input matrix ... \n");
#endif

//read binary matrix (entries 2 or 3 will be converted to 1)
  ALLOC(M,bm,1);
  status = readbinarymatrix(fptr,M);

  if(status==BINARYMATRIX_NONBINARY) {
    fprintf(stderr,"Error: Not a binary matrix: Only 0,1 and newlines are allowed.\n");
    return(EXIT_FAILURE);
  }
  if(status==BINARYMATRIX_NOTSAMELENGTH) {
    fprintf(stderr,"Error: Not all rows have same length.\n");
    return(EXIT_FAILURE);
  }
  if(status==BINARYMATRIX_TOOMANYCOLUMNS) {
    fprintf(stderr,"Error: Input matrix too large (max %d columns allowed).\n",
            BINARYMATRIX_MAXCOLUMNS);
    return(EXIT_FAILURE);
  }
  if(status==BINARYMATRIX_TOOMANYROWS) {
    fprintf(stderr,"Error: Input matrix too large (max %d rows allowed).\n",
            BINARYMATRIX_MAXROWS);
    return(EXIT_FAILURE);
  }

//close file
  FCLOSE(fptr);

#ifdef LOG
  fprintf(stderr,"done (n=%d,k=%d)\n",M->n,M->k);
#endif

#ifdef DEBUG
  printbinarymatrix(stderr,M);
#endif

  if(M->k==1) {
    fprintf(stderr,"Warning: A single sequence never contains maximal haplotype blocks.\n");
    return(EXIT_SUCCESS);
  }

//create initial tree
#ifdef LOG
  fprintf(stderr,"Creating initial tree:\n");
#endif
  ALLOC(root,lazynode,1);
  root->parent = NULL;
  root->w = 0;
  root->num = 1;
  for(i=1; i<M->k; i++) {
    add(root,i,M);
#ifdef LOG
    if((i+1)%100==0) {
      fprintf(stderr,"%d lines processed\n",i+1);
    }
#endif
  }

#ifdef DEBUG
  printtree(stderr,root,0,M);
#endif

#ifdef LOG
  fprintf(stderr,"Finding maximal submatrices ...\n");
#endif
  findmax(stdout,root,t,M);
#ifdef LOG
  fprintf(stderr,"done\n");
#endif

//advance through matrix
#ifdef LOG
  fprintf(stderr,"Advancing through matrix ...\n");
#endif
  for(j=1; j<M->n; j++) {
    root = advance(stdout,root,j,t,M);
#ifdef LOG
    if((j+1)%100==0) {
      fprintf(stderr," %d columns processed\n",j+1);
    }
    else if((j+1)%10==0) {
      fprintf(stderr,".");
    }
#endif
#ifdef DEBUG
    printtree(stderr,root,j,M);
#endif
  }
  outputsubmatrix(stdout,root,M->n,0,t,M);
#ifdef LOG
  fprintf(stderr,"done\n");
#endif

  //free memory
  if(root->left!=NULL) {
    freenode(root->left);
  }
  if(root->right!=NULL) {
    freenode(root->right);
  }
  freenode(root);
  freebinarymatrix(M);

  return(EXIT_SUCCESS);
}
/****** EOF (haploblocks.c) **************************************************/
