#ifndef KD_SKIPLIST
#define KD_SKIPLIST

#include "contest_interface.h"
#include "skiplist.h"
#include <utility>
#include <string>
#include <unordered_map>
#include <set>
#include <map>

using namespace std;

struct Iterator {
  int current_list;
  Transaction *transaction;
  skiplist **lists;
};

struct Transaction {
  set< skipnode* > *deleted_nodes;
  set< skipnode* > *inserted_nodes;
  map< skipnode*, Block* > *updates; // contains already copied data
};

struct Index {
  uint8_t attribute_count;
  string *name;
  KeyType *type;
  skiplist *list;
};

#endif /* KD_SKIPLIST */