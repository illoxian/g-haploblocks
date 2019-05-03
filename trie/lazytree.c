/*****************************************************************************
 *  FILE   : lazytree.c
 *  AUTHOR : Jens Stoye
 *  DATE   : 06-09/2018
 *****************************************************************************
 *
 *****************************************************************************/

/*****************************************************************************
 * SWITCHES
 *****************************************************************************/
//#define DEBUG

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdio.h>

#include "types.h"
#include "alloc.h"

#include "binarymatrix.h"
#include "lazytree.h"

/*****************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************/
lazynode *newChild(lazynode *parent,Ushrt w) {
  lazynode *child;

  ALLOC(child,lazynode,1);
  child->parent = parent;
  child->w = w;
  child->num = 1;
  return child;
}

/*---------------------------------------------------------------------------*/
void evaluatenode(lazynode *n,Uint d,bm *M) {
  lazynode *child;

  child = newChild(n,n->w);
  if(MATRIXENTRY(M,n->w,d)) {
    n->right = child;
    n->left = NULL;
  }
  else {
    n->right = NULL;
    n->left = child;
  }
  return;
}

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
void outputsubmatrix(FILE *fptr,lazynode *n,Uint offset,Uint d,Uint t,bm *M) {
  Uint weight=d*n->num;

  if(weight>=t) {
    fprintf(fptr,"%u (%u-%u,%u:%u)",weight,offset,offset+d-1,n->w,n->num);
#ifdef DEBUG
    listleaves(fptr,n,offset+d,M);
#endif
    printf("\n");
  }
  return;
}

/*---------------------------------------------------------------------------*/
void add(lazynode *root,Ushrt w,bm *M) {
  lazynode *current,**nextnodeptr=&root;
  Uint d=0;

  //down, down, down ... (while incrementing num on the path)
  do {
    current = *nextnodeptr;
    if(d<M->n && current->num==1) {
      evaluatenode(current,d,M);
    }
    current->num++;
    nextnodeptr = MATRIXENTRY(M,w,d) ? &current->right : &current->left;
    d++;
  } while(d<M->n && *nextnodeptr!=NULL);

  //create new child (or increment num at leaf)
  if(*nextnodeptr==NULL) {
    (*nextnodeptr) = newChild(current,w);
  }
  else {
    (*nextnodeptr)->num++;
  }

  return;
}

/*---------------------------------------------------------------------------*/
void findmax(FILE *fptr,lazynode *root,Uint t,bm *M) {
  lazynode *n=root;
  Uint d=0;

  while(1) {
    //down, down, down ...
    while(d<M->n && n->num>1) {
      n = n->left==NULL ? n->right : n->left;
      d++;
    }

    //process leaf
    if(d==M->n && n->num>1) {
      outputsubmatrix(fptr,n,0,d,t,M);
    }

    //up, up, up ... and one down (or return at root)
    while((n==n->parent->left && n->parent->right==NULL) || n==n->parent->right) {
      n = n->parent;
      if(n==root) {
        return;
      }
      d--;
    }
    n = n->parent->right;

    //process branching vertex n->parent
    outputsubmatrix(fptr,n->parent,0,d-1,t,M);
  }
}

/*---------------------------------------------------------------------------*/
lazynode *advance(FILE *fptr,lazynode *root,Uint offset,Uint t,bm *M) {
  lazynode *l,*r,*prevl,*prevr,*lroot,*rroot;
  Uint d=0;

  //merge left and right subtrees of root into left subtree
  prevl = prevr = root;
  l = lroot = root->left;
  r = rroot = root->right;
  if(l==NULL) {
    return r;
  }
  if(r==NULL) {
    return l;
  }

  while(1) {
    //down, down, down ...
    while(offset+d<M->n && l!=NULL && r!=NULL) {
      if(l->num==1) {
        evaluatenode(l,offset+d,M);
      }
      if(r->num==1) {
        evaluatenode(r,offset+d,M);
      }
      prevl = l;
      l = l->left;
      prevr = r;
      r = r->left;
      d++;
    }

    //process leaf or case that one left subtree is empty
    if(l==NULL && r!=NULL) {
      prevl->left = r;
      r->parent = prevl;
    }
    else if(l!=NULL && r!=NULL && offset+d==M->n) {
      l->num += r->num;
      outputsubmatrix(fptr,l,offset,d,t,M);
      freenode(r);
    }

    //up, up, up ... and one down (or return at root)
    while((l==prevl->left && prevl->right==NULL) || l==prevl->right) {
      prevl->num = (prevl->left==NULL ? 0 : prevl->left->num) + (prevl->right==NULL ? 0 : prevl->right->num);

      if(l==prevl->left && prevl->right==NULL && prevr->right!=NULL) {
        prevl->right = prevr->right;
        prevl->num += prevl->right->num;
        prevr->right->parent = prevl;
        outputsubmatrix(fptr,prevl,offset,d-1,t,M);
      }
      else if(l==prevl->right && prevl->left!=NULL) {
        outputsubmatrix(fptr,prevl,offset,d-1,t,M);
      }
      l = prevl;
      r = prevr;
      prevl = l->parent;
      prevr = r->parent;
      freenode(r);
      if(l==lroot) {
        freenode(root);
        return lroot;
      }
      d--;
    }
    l = prevl->right;
    r = prevr->right;
  }
}

/*---------------------------------------------------------------------------*/
void listleaves(FILE *fptr,lazynode* n,Uint d,bm *M) {
  lazynode *current=n;

  if(d==M->n) {
    fprintf(fptr," %d:%d",current->w+1,current->num);
    return;
  }

  while(1) {
    //down, down, down ...
    while(d<M->n && current->num>1) {
      if(current->left!=NULL) {
        current = current->left;
      }
      else {
        current = current->right;
      }
      d++;
    }

    //process leaf or unevaluated node
    fprintf(fptr," %d:%d",current->w+1,current->num);

    //up, up, up ... and one down (or return at start vertex n)
    while((current==current->parent->left && current->parent->right==NULL) || current==current->parent->right) {
      current = current->parent;
      if(current==n) {
        return;
      }
      d--;
    }
    current = current->parent->right;
  }
}

/*---------------------------------------------------------------------------*/
void printtree(FILE *fptr,lazynode *n,Uint d,bm *M) {
  lazynode *current=n;

  if(d==M->n) {
    fprintf(fptr,"*%d:%d\n",current->w+1,current->num);
    return;
  }

  while(1) {
    //down, down, down ...
    while(d<M->n && current->num>1) {
      if(current->left!=NULL) {
        fprintf(fptr,"0");
        current = current->left;
      }
      else {
        fprintf(fptr,"1");
        current = current->right;
      }
      d++;
    }

    //process leaf or unevaluated node
    if(d==M->n) {
      fprintf(fptr,"*%d:%d\n",current->w+1,current->num);
    }
    else { //unevaluated node
      fprintf(fptr,"+%c...:%d:1\n",'0'+MATRIXENTRY(M,current->w,d),current->w+1);
    }

    //up, up, up ... and one down (or return at start vertex n)
    while((current==current->parent->left && current->parent->right==NULL) || current==current->parent->right) {
      current = current->parent;
      if(current==n) {
        return;
      }
      d--;
    }
    fprintf(fptr,"1");
    current = current->parent->right;
  }
}

/*---------------------------------------------------------------------------*/
void freenode(lazynode *n) {
  if(n!=NULL) {
    FREE(n);
  }
  return;
}

/****** EOF (lazytree.c) *****************************************************/
