#ifndef NAIVE_SKIPLIST
#define NAIVE_SKIPLIST

#include "contest_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define NODEHEIGHT 32

typedef struct skipnode skipnode;
typedef struct skiplist skiplist;

struct skipnode {
  skipnode *ptr[NODEHEIGHT];
  Attribute attr;
  Record rec; // we are supposed to copy the record into the index so that its immutable
};

struct skiplist {
  int n;
  int maxlevel;
  double p;
  AttributeType type;
  skipnode *start;
};

skipnode* createNode(Attribute attr);
skiplist* createList(AttributeType type);
double randDouble();

skipnode* findNode(skiplist *list, Attribute attr);
skipnode* findNodeAndTrack(skiplist *list, Attribute attr, skipnode **path);
ErrorCode insert(skiplist *list, Attribute attr);
void printtList(skiplist *list);
void printAttribute(Attribute attr, int brackets);
int compareAttributes(Attribute l, Attribute r);

#endif /* NAIVE_SKIPLIST */