#ifndef NAIVE_SKIPLIST
#define NAIVE_SKIPLIST

#include "contest_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define NODEHEIGHT 10

typedef struct skipnode skipnode;
typedef struct skiplist skiplist;

struct skipnode {
  skipnode *ptr[NODEHEIGHT];
  int element;
};

struct skiplist {
  int n;
  int maxlevel;
  double p;
  skipnode *start;
};

skipnode* createNode(int element);
skiplist* createList();
double randDouble();

skipnode* findNode(skiplist *list, int element);
skipnode* findNodeAndTrack(skiplist *list, int element, skipnode **path);
int insert(skiplist *list, int element);
void printtList(skiplist *list);

#endif /* NAIVE_SKIPLIST */