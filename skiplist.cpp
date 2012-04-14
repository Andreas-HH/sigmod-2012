#include "skiplist.h"
#include "skiplist.h"

skipnode* findNodeAndTrack(skiplist *list, Key key, skipnode **path);
double randDouble();
void printAttribute(Attribute attr, int brackets);
int compareKeys(Key l, Key r, int num_attribute);
int compareAttributes(Attribute l, Attribute r);

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
    node->right[i] = 0;
    node->next_dim[i] = 0;
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
skiplist* createList(KeyType type, int dim, int attribute_count) {
//   int i;
  Key key;
  skiplist *list;
  
//   if (dim == attribute_count) return 0;
  
  list = (skiplist*) malloc(sizeof(skiplist));
  list->start = createNode(key, 0);
  list->p = 0.5;
  list->n = 0;
  list->maxlevel = 1;
  list->type = type;
  list->num_attribute = dim;
  list->attribute_count = attribute_count;
  
//   for (i = 0; i < NODEHEIGHT-1; i++) {
//     list->start->next_dim[i] = createList(type, dim+1, attribute_count);
//   }
  
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
    current = last->right[0];
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
    while (current->right[level] != 0 && compareKeys(current->right[level]->key, key, list->num_attribute) < 0) {
      current = current->right[level];
//       printf("compare < 0! \n");
//       depth++;
    }
    path[level] = current;
  }
//   printf("found something with depth %i \n", depth);
  return current;
}

skipnode* nextNode(skipnode* current) {
  return current->right[0];
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
//   printf(" %i \n", position->right[2]);
//   if (position == list->start) printf("found start position! \n");
//   if (position->element == element) return 1;
  node->right[0] = position->right[0];
  position->right[0] = node;
  for (i = 1; i < NODEHEIGHT; i++) {
    if (randDouble() < list->p) {
      node->right[i] = path[i]->right[i];
      path[i]->right[i] = node;
    } else {
      break;
    }
  }
  // insert record into higher-level nodes
  for ( ; i < list->maxlevel; i++) {
    if (path[i]->next_dim[i] != 0)
      insert(path[i]->next_dim[i], key, rec, insnode);
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
      if (current->right[list->maxlevel - i - 1] == 0) printf("[x]\t");
      else printAttribute(*current->right[list->maxlevel - i - 1]->key.value[list->num_attribute], 1);
      current = current->right[0];
    }
    printf("\n");
  }
  printf(" .\t");
  current = list->start->right[0];
  while (current != 0) {
    printf("%s,", current->record->payload.data);
    printAttribute(*current->key.value[list->num_attribute], 0);
    current = current->right[0];
  }
  printf("\n");
}

int compareAttributes(Attribute l, Attribute r) {
  int i;
  
  switch (l.type) {
    case kShort:
      if (l.short_value < r.short_value)      return -1;
      else if (l.short_value > r.short_value) return 1;
      break;
    case kInt:
      if (l.int_value <= r.int_value)     return -1;
      else if (l.int_value > r.int_value) return 1;
      break;
    case kVarchar:
      for (i = 0; i < MAX_VARCHAR_LENGTH; i++) {
	if (l.char_value[i] == r.char_value[i]) continue;
	if (l.char_value[i] < r.char_value[i])  return -1;
	else                                    return 1;
      }
      break;
  }
  
  return 0;
}

int compareKeys(Key left, Key right, int num_attribute) {
  int i;
  Attribute l = *left.value[num_attribute];
  Attribute r = *right.value[num_attribute];
  int compare;
  
//   printf("comparing attributes: ");
//   printAttribute(l, 0);
//   printAttribute(r, 0);
//   printf("\n");
  compare = compareAttributes(l, r);
  
  if (compare == 0) {
    for (i = 0; i < left.attribute_count; i++) {
      if (i == num_attribute) continue;
      l = *left.value[i];
      r = *right.value[i];
      compare = compareAttributes(l, r);
      if (compare != 0) break;
    }
  }
  
  return compare;
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
