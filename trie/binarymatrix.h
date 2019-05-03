/*****************************************************************************
 *  FILE   : binarymatrix.h
 *  AUTHOR : Jens Stoye
 *  DATE   : 09/2018
 *****************************************************************************/
#ifndef BINARYMATRIX_H
#define BINARYMATRIX_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <limits.h>
#include "types.h"

/*****************************************************************************
 * CONSTANTS
 *****************************************************************************/
#define ONES "123456789abcdefghijklmnopqrstuvwxyz"
#define BINARYMATRIX_MAXCOLUMNS UINT_MAX
#define BINARYMATRIX_MAXROWS USHRT_MAX
#define BINARYMATRIX_ALLOCSTEP 1048576

/*****************************************************************************
 * MACROS
 *****************************************************************************/
#define MATRIXENTRY(M,i,j) ((M->rows[i][j/8]>>(j%8))&1)

/*****************************************************************************
 * TYPE DEFINITIONS
 *****************************************************************************/
typedef enum {BINARYMATRIX_SUCCESS,BINARYMATRIX_NONBINARY,BINARYMATRIX_NOTSAMELENGTH,BINARYMATRIX_TOOMANYCOLUMNS,BINARYMATRIX_TOOMANYROWS} binarymatrixstatus;
typedef struct BinaryMatrix {
  Uchar **rows;
  Ushrt k;
  Uint n;
} bm;

/*****************************************************************************
 * FUNCTION DECLARATIONS
 *****************************************************************************/
binarymatrixstatus readbinarymatrix(FILE *fptr,bm *M);
void printbinarymatrix(FILE *fptr,bm *M);
void freebinarymatrix(bm *M);

#endif /* BINARYMATRIX_H */
/****** EOF (binarymatrix.h) *************************************************/
