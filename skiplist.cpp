#include "skiplist.h"
#include "skiplist.h"

skipnode* findNodeAndTrack(skiplist *list, Key key, skipnode **path);
double randDouble();
void printAttribute(Attribute attr, int brackets);
int compareKeys(Key l, Key r, int num_attribute);


double randDouble() {
  return (double) rand() / (double) RAND_MAX;
}

int randInt(int limit) {
  return (int) (randDouble() * (double) limit);
}


skipnode* createNode(Key key, Record* rec) {
  int i;
  skipnode *node = (skipnode*) malloc(sizeof(skipnode));
  
  for (i = 0; i < NODEHEIGHT; i++) {
    node->ptr[i] = 0;
  }
  node->committed = 0;
  if (rec != 0) {
//     printf("Copying some record \n");
    node->record = (Record*) malloc(sizeof(Record));
    memcpy(node->record, rec, sizeof(Record));
    node->key = key;
  }
  
  return node;
}

// need to find min properly, depending on type!
skiplist* createList(KeyType type, int dim) {
  Key key;
  skiplist *list = (skiplist*) malloc(sizeof(skiplist));
  list->start = createNode(key, 0);
  list->p = 0.5;
  list->n = 0;
  list->maxlevel = 1;
  list->type = type;
  list->num_attribute = dim;
  
  return list;
}

ErrorCode deleteNode(skipnode* node) {
  free(node);
  
  return kOk;
}

ErrorCode deleteList(skiplist* list) {
  skipnode *current, *last;
  
  last = list->start;
  while (last != 0) {
    current = last->ptr[0];
    deleteNode(last);
    last = current;
  }
//   printf("deleting some list \n");
  free(list);
  
  return kOk;
}

skipnode* findNode(skiplist* list, Key key) {
  skipnode *path[NODEHEIGHT];
  
  return findNodeAndTrack(list, key, path);
}

skipnode* findNodeAndTrack(skiplist* list, Key key, skipnode** path) {
//   int depth = 0;
  int level = list->maxlevel;
  skipnode *current = list->start;
  
  while (level > 0) {
    level--;
    while (current->ptr[level] != 0 && compareKeys(current->ptr[level]->key, key, current->num_attribute) < 0) {
      current = current->ptr[level];
//       printf("compare < 0! \n");
//       depth++;
    }
    path[level] = current;
  }
//   printf("found something with depth %i \n", depth);
  return current;
}

skipnode* nextNode(skipnode* current) {
  return current->ptr[0];
}


ErrorCode insert(skiplist* list, Key key, Record* rec, skipnode **insnode) {
  int i;
  skipnode *node = createNode(key, rec);
  skipnode *position;
  skipnode *path[NODEHEIGHT];
  
//   if (attr.type != list->type) return kErrorIncompatibleKey;
  
  for (i = 0; i < NODEHEIGHT; i++) {
    path[i] = list->start;
  }
  
  position = findNodeAndTrack(list, key, path);
//   printAttribute(attr, 1);
//   printf(" %i \n", position->ptr[2]);
//   if (position == list->start) printf("found start position! \n");
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
  *insnode = node;
  
  return kOk;
}

// maybe want to use an iterator here?
void printList(skiplist* list) {
  int i;
  skipnode *current;// = list->start;
  
  for (i = 0; i < list->maxlevel; i++) {
    current = list->start;
    while (current != 0) {
      if (current->ptr[list->maxlevel - i - 1] == 0) printf("[x]\t");
      else printAttribute(*current->ptr[list->maxlevel - i - 1]->key.value[list->num_attribute], 1);
      current = current->ptr[0];
    }
    printf("\n");
  }
  printf(" .\t");
  current = list->start->ptr[0];
  while (current != 0) {
    printf("%s,", current->record->payload.data);
    printAttribute(*current->key.value[list->num_attribute], 0);
    current = current->ptr[0];
  }
  printf("\n");
}

/**
 * Compares l and r, return 0 if they are incomparable, -1 if l <= r and 1 otherwise.
 */
int compareKeys(Key left, Key right, int num_attribute) {
  int i;
  Attribute l = *left.value[num_attribute];
  Attribute r = *right.value[num_attribute];
  
//   printf("comparing attributes: ");
//   printAttribute(l, 0);
//   printAttribute(r, 0);
//   printf("\n");
  
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
//       printf("short...");
      printf("%i", attr.short_value);
      break;
    case kInt:
//       printf("int...");
      printf("%ld", attr.int_value);
      break;
    case kVarchar:
      printf("%s", attr.char_value);
      break;
  }
  if (brackets) printf("]");
  printf("\t");
}

// int main(int argc, char *argv[]) {
//   int i;
//   AttributeType type = kInt;
//   Attribute a = {type, -3};
//   skiplist *list = createList(type);
//   skipnode *node;
//   Record *rec = (Record*) malloc(sizeof(Record));
//   
//   srand (time(NULL));
//   
//   a.int_value = 5;
//   insert(list, a, rec);
//   a.int_value = 97;
//   insert(list, a, rec);
//   a.int_value = 15;
//   insert(list, a, rec);
//   a.int_value = 33;
//   insert(list, a, rec);
//   a.int_value = 42;
//   insert(list, a, rec);
//   a.int_value = 2;
//   insert(list, a, rec);
//   a.int_value = 11;
//   insert(list, a, rec);
//   a.int_value = 77;
//   insert(list, a, rec);
//   a.int_value = 57;
//   insert(list, a, rec);
//   a.int_value = 9;
//   insert(list, a, rec);
//   
//   printtList(list);
//   
//   return 0;
// }
