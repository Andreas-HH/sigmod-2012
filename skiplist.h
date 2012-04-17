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
#include <set>

#define NODEHEIGHT 20
#define MAX_DIM    4

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
  int64_t hans;
};

struct skiplist {
  int num;
  int n;
  int maxlevel;
  int max_dim; // maximum indexed dimension
  double p;
  int num_attribute;
  int attribute_count;
//   AttributeType type;
  AttributeType *type;
  skipnode *start;
};

skipnode* createNode(Key key, Record* rec);
skiplist* createList(KeyType type, int dim, int attribute_count, int max_dim);
ErrorCode deleteNode(skipnode *node);
ErrorCode deleteList(skiplist *list);

skipnode* findNode(skiplist* list, Key key);
skipnode* nextNode(skipnode *current, Key key);

ErrorCode insert(skiplist *list, Key key, Record* rec, skipnode** insnode);
ErrorCode deleteRecord(skiplist *list, Key key);
ErrorCode findRecords(skiplist* list, Key minkey, Key maxkey, multimap< Key, skipnode* >* result);

// int compareKeyElements(Key left, Key right, int num_attribute);
int compareKeys(Key left, Key right, int num_attribute = 0, bool elementOnly = false);
void printAllLists();
void printList(skiplist *list);
int randInt(int limit);

bool operator<(Key l, Key r);
// struct lkey
// {
//   bool operator()(Key l, Key r) 
//   {
//     return compareKeys(l, r) < 0;
//   }
// };

#endif /* NAIVE_SKIPLIST */