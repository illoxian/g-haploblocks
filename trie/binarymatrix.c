/*****************************************************************************
 *  FILE   : binarymatrix.c
 *  AUTHOR : Jens Stoye
 *  DATE   : 09/2018
 *****************************************************************************
 *
 *****************************************************************************/

/*****************************************************************************
 * SWITCHES
 *****************************************************************************/
//#define LOG

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "binary.h"
#include "alloc.h"

#include "binarymatrix.h"

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/

binarymatrixstatus readbinarymatrix(FILE *fptr,bm *M) {
  Uint len,binlen;
  Uchar c,cbin,count;

  M->k = 0;
  ALLOC(M->rows,Uchar*,1);
  len = 0;
  ALLOC(M->rows[M->k],Uchar,1);
  while(!feof(fptr)) {
    c = fgetc(fptr);
    while(!feof(fptr) && (c=='0' || strchr(ONES,c)!=NULL)) {
      cbin = 0;
      count = 0;
      while(!feof(fptr) && (c=='0' || strchr(ONES,c)!=NULL) && count<8) {
        if(strchr(ONES,c)!=NULL) {
          cbin |= ONE(count);
        }
        count++;
        c = fgetc(fptr);
      }
      if(len%BINARYMATRIX_ALLOCSTEP == 0) { /* allocate more memory */
        if(len>BINARYMATRIX_MAXCOLUMNS-BINARYMATRIX_ALLOCSTEP) {
          return(BINARYMATRIX_TOOMANYCOLUMNS);
        }
        REALLOC(M->rows[M->k],Uchar,len+BINARYMATRIX_ALLOCSTEP);
      }
      M->rows[M->k][len++] = cbin;
    }
    binlen = (len-1)*8+count;

    if(!feof(fptr)) {
      if(len==0) {
        return(BINARYMATRIX_SUCCESS);
      }
      if(c!='\n') {
        return(BINARYMATRIX_NONBINARY);
      }
      if(M->k>0 && len!=0 && binlen!=M->n) {
        return(BINARYMATRIX_NOTSAMELENGTH);
      }
      if(M->k==BINARYMATRIX_MAXROWS) {
        return(BINARYMATRIX_TOOMANYROWS);
      }
      M->k++;
      REALLOC(M->rows,Uchar*,M->k+1);
      M->n = binlen;
      len = 0;
      ALLOC(M->rows[M->k],Uchar,1);
#ifdef LOG
      if(M->k%100==0) {
        fprintf(stderr,"%d lines processed\n",M->k);
      }
#endif
    }
  }
  return(BINARYMATRIX_SUCCESS);
}

/*---------------------------------------------------------------------------*/
void printbinarymatrix(FILE *fptr,bm *M) {
  Ushrt i;
  Uint j;

  for(i=0; i<M->k; i++) {
    for(j=0; j<M->n/8; j++) {
      fprintf(fptr,"%s",SHOWBINREVERSE(M->rows[i][j]));
    }
    for(j*=8; j<M->n; j++) {
      fprintf(fptr,"%c",'0'+(MATRIXENTRY(M,i,j)));
    }
    fprintf(fptr,"\n");
  }
  return;
}

/*---------------------------------------------------------------------------*/
void freebinarymatrix(bm *M) {
  Ushrt i;

  for(i=0; i<=M->k; i++) {
    FREE(M->rows[i]);
  }
  free(M->rows);
  free(M);
  return;
}

/****** EOF (binarymatrix.c) *************************************************/
