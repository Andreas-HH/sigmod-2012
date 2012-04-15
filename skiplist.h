#ifndef NAIVE_SKIPLIST
#define NAIVE_SKIPLIST

#include "contest_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <map>

#define NODEHEIGHT 20
#define MAX_DIM    32

using namespace std;

typedef struct skipnode skipnode;
typedef struct skiplist skiplist;

struct skipnode {
//   skipnode *ptr[NODEHEIGHT];
  skipnode *right[NODEHEIGHT];
//   skipnode *down;
  skiplist *next_dim[NODEHEIGHT]; // no next dim for leaf nodes
  int num_attribute;
  Key key;
  Record *record; // we are supposed to copy the record into the index so that its immutable
  int committed;
};

struct skiplist {
  int num;
  int n;
  int maxlevel;
  double p;
  int num_attribute;
  int attribute_count;
//   AttributeType type;
  AttributeType *type;
  skipnode *start;
};

skipnode* createNode(Key key, Record* rec);
skiplist* createList(KeyType type, int dim, int attribute_count);
skipnode* findNode(skiplist* list, Key key);
skipnode* nextNode(skipnode *current, Key key);
ErrorCode insert(skiplist* list, Key key, Record* rec, skipnode** insnode);
ErrorCode deleteNode(skipnode *node);
ErrorCode deleteList(skiplist *list);

void printAllLists();
void printList(skiplist *list);
int randInt(int limit);

#endif /* NAIVE_SKIPLIST */