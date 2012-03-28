#include "naive_skiplist.h"

double randDouble() {
  return (double) rand() / (double) RAND_MAX;
}


skipnode* createNode(Attribute attr) {
  int i;
  skipnode *node = (skipnode*) malloc(sizeof(skipnode));
  
  for (i = 0; i < NODEHEIGHT; i++) {
    node->ptr[i] = 0;
  }
  node->attr = attr;
  
  return node;
}

skiplist* createList(AttributeType type) {
  skiplist *list = (skiplist*) malloc(sizeof(skiplist));
  Attribute a = {type, INT_MIN};
  list->start = createNode(a);
  list->p = 0.5;
  list->n = 0;
  list->maxlevel = 1;
  list->type = type;
  
  return list;
}

skipnode* findNode(skiplist* list, Attribute attr) {
  skipnode *path[NODEHEIGHT];
  
  return findNodeAndTrack(list, attr, path);
}

skipnode* findNodeAndTrack(skiplist* list, Attribute attr, skipnode** path) {
//   int depth = 0;
  int level = list->maxlevel;
  skipnode *current = list->start;
  
  while (level > 0) {
    level--;
    while (current->ptr[level] != 0 && compareAttributes(current->ptr[level]->attr, attr) < 0) {
      current = current->ptr[level];
//       depth++;
    }
    path[level] = current;
  }
//   printf("found something with depth %i \n", depth);
  return current;
}

ErrorCode insert(skiplist* list, Attribute attr) {
  int i;
  skipnode *node = createNode(attr);
  skipnode *position;
  skipnode *path[NODEHEIGHT];
  
  if (attr.type != list->type) return kErrorIncompatibleKey;
  
  for (i = 0; i < NODEHEIGHT; i++) {
    path[i] = list->start;
  }
  
  position = findNodeAndTrack(list, attr, path);
//   if (position->element == element) return 1;
  node->ptr[0] = position->ptr[0];
  position->ptr[0] = node;
  for (i = 1; i < NODEHEIGHT; i++) {
    if (randDouble() < list->p) {
      node->ptr[i] = path[i]->ptr[i];
      path[i]->ptr[i] = node;
    } else {
      break;
    }
  }
  if (i > list->maxlevel) list->maxlevel = i;
  list->n++;
  
  return kOk;
}

// maybe want to use an iterator here?
void printtList(skiplist* list) {
  int i;
  skipnode *current;// = list->start;
  
  for (i = 0; i < list->maxlevel; i++) {
    current = list->start;
    while (current != 0) {
      if (current->ptr[list->maxlevel - i - 1] == 0) printf("[x]\t");
      else printAttribute(current->ptr[list->maxlevel - i - 1]->attr, 1);
      current = current->ptr[0];
    }
    printf("\n");
  }
  printf(" .\t");
  current = list->start->ptr[0];
  while (current != 0) {
    printAttribute(current->attr, 0);
    current = current->ptr[0];
  }
  printf("\n");
}

/**
 * Compares l and r, return 0 if they are incomparable, -1 if l <= r and 1 otherwise.
 */
int compareAttributes(Attribute l, Attribute r) {
  int i;
  
  if (l.type != r.type) return 0;
  switch (l.type) {
    case kShort:
      if (l.short_value <= r.short_value) return -1;
      else                                return 1;
      break;
    case kInt:
      if (l.int_value <= r.int_value) return -1;
      else                            return 1;
      break;
    case kVarchar:
      for (i = 0; i < MAX_VARCHAR_LENGTH; i++) {
	if (l.char_value[i] == r.char_value[i]) continue;
	if (l.char_value[i] < r.char_value[i])  return -1;
	else                                    return 1;
      }
      if (i == MAX_VARCHAR_LENGTH) return -1; // -1 in case of equality
      break;
  }
  return 0;
}

void printAttribute(Attribute attr, int brackets) {
  if (brackets) printf("[");
  switch (attr.type) {
    case kShort:
      printf("%i", attr.short_value);
      break;
    case kInt:
      printf("%ld", attr.int_value);
      break;
    case kVarchar:
      printf("%s", attr.char_value);
      break;
  }
  if (brackets) printf("]");
  printf("\t");
}

int main(int argc, char *argv[]) {
  int i;
  AttributeType type = kInt;
  Attribute a = {type, -3};
  skiplist *list = createList(type);
  skipnode *node;
  
  srand (time(NULL));
  
  a.int_value = 5;
  insert(list, a);
  a.int_value = 97;
  insert(list, a);
  a.int_value = 15;
  insert(list, a);
  a.int_value = 33;
  insert(list, a);
  a.int_value = 42;
  insert(list, a);
  a.int_value = 2;
  insert(list, a);
  a.int_value = 11;
  insert(list, a);
  a.int_value = 77;
  insert(list, a);
  a.int_value = 57;
  insert(list, a);
  a.int_value = 9;
  insert(list, a);
  
  printtList(list);
  
  return 0;
}
