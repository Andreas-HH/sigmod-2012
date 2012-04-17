#include "skiplist.h"

int skip_number = 0;
map< int, skiplist* > all_lists;

skipnode* findNodeAndTrack(skiplist *list, Key key, skipnode **path, bool ignoreKey = false, bool elementOnly = false);
double randDouble();
void printAttribute(Attribute attr, int brackets = 0);
int compareAttributes(Attribute l, Attribute r);
void fillNextDim(skiplist* list, skipnode* owner, skipnode* limit, int height);

double randDouble() {
  return (double) rand() / (double) RAND_MAX;
}

int randInt(int limit) {
  return (int) (randDouble() * (double) limit);
}

bool operator<(Key l, Key r) {
  return compareKeys(l, r) < 0;
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
    node->hans = key.value[5]->int_value;
//     printf("Copying some record \n");
    node->record = (Record*) malloc(sizeof(Record));
    memcpy(node->record, rec, sizeof(Record));
    node->key = key;
  }
  
  return node;
}

// need to find min properly, depending on type!
skiplist* createList(KeyType type, int dim, int attribute_count, int max_dim) {
  int i;
  Key key;
//   AttributeType *kt;
  skiplist *list;
  
  if (dim == max_dim) return 0; // attribute_count
  
//   kt = (AttributeType*) malloc(attribute_count*sizeof(AttributeType));
//   for (i = 0; i < attribute_count; i++) {
//     kt[i] = type[i];
//   }
  list = (skiplist*) malloc(sizeof(skiplist));
  list->num = skip_number++;
  list->start = createNode(key, 0);
  list->p = 0.5;
  list->n = 0;
  list->maxlevel = 1;
  list->type = type;
  list->num_attribute = dim;
  list->attribute_count = attribute_count;
  list->max_dim = max_dim;
  
  all_lists[list->num] = list;
  
//   for (i = 0; i < NODEHEIGHT-1; i++) {
//     list->start->next_dim[i] = createList(type, dim+1, attribute_count);
//   }
  
  return list;
}

ErrorCode deleteNode(skipnode* node) {
  int i;
  
  for(i = 0; i < NODEHEIGHT; i++) {
    if (node->next_dim[i] != 0) {
      deleteList(node->next_dim[i]);
    }
  }
  
  free(node);
  
  return kOk;
}

ErrorCode deleteList(skiplist* list) {
  skipnode *current, *last;
  
  if (list == 0) return kErrorNotFound;
  
  last = list->start;
  while (last != 0) {
    current = last->right[0];
    deleteNode(last);
    last = current;
  }
//   printf("deleting some list (%i) \n", list->num);
  all_lists[list->num] = 0;
  free(list);
  
  return kOk;
}

skipnode* findNode(skiplist* list, Key key) {
  skipnode *path[NODEHEIGHT];
  
  return findNodeAndTrack(list, key, path);
}

/**
 * Finds the node left of the one with the given key or the node with largest key that is smaller than the given key.
 */
skipnode* findNodeAndTrack(skiplist* list, Key key, skipnode** path, bool ignoreKey, bool elementOnly) {
//   int depth = 0;
  int level = list->maxlevel;
  skipnode *current = list->start;
  
//   printf("find and track: maxlevel = %i \n", list->maxlevel);
//   if (current->right[0] != 0) {
//     printf("   xxx   ");
//     printAttribute(*current->right[0]->key.value[list->num_attribute]);
//     printf("   xxx   \n");
//   }
  
  while (level > 0) {
    level--;
    while (current->right[level] != 0) {
      if (ignoreKey || compareKeys(current->right[level]->key, key, list->num_attribute, elementOnly) < 0) {
        current = current->right[level];
      } else {
	break;
      }
//       printf("compare < 0! \n");
//       depth++;
    }
    path[level] = current;
  }
//   printf("found something with depth x \n");
  return current;
}

skipnode* nextNode(skipnode* current) {
  return current->right[0];
}

ErrorCode findRecords(skiplist* list, Key minkey, Key maxkey, multimap< Key, skipnode* > *result) {
  int i, j;
  skipnode *minnode;
  skipnode *maxnode;
  skipnode *current;
  skipnode *minpath[NODEHEIGHT];
  skipnode *maxpath[NODEHEIGHT];
  
  for (i = 0; i < NODEHEIGHT; i++) {
    minpath[i] = list->start;
    maxpath[i] = list->start;
  }
  
  // if NULL is given here, just stay at the beginning
  if (minkey.value[list->num_attribute] != 0) {
    minnode = findNodeAndTrack(list, minkey, minpath, false, true);
  }
  // if NULL is given here, search for the end of the list
  if (minkey.value[list->num_attribute] != 0) {
    maxnode = findNodeAndTrack(list, maxkey, maxpath, false, true);
  } else {
    maxnode = findNodeAndTrack(list, maxkey, maxpath, true, true);
  }
  
  i = list->maxlevel-1;
  while (minpath[i] == maxpath[i] && i > 0) { // i=0 means minkey = maxkey
    i--;
  }
  
  if (i == 0 || minpath[i]->next_dim[i-1] == 0) { // reached leaf level or last dimension, just scan nodes
    current = minnode->right[0]; // remember, findNode is always one to the left
    while (current != 0 && compareKeys(current->key, maxkey, list->num_attribute, true) <= 0) {
      for (j = 0; j < 5; j++) { // len
        if (!(compareKeys(minkey, current->key, j, true) <= 0 && compareKeys(current->key, maxkey, j, true) < 0)) {
	  break;
        }
      }
      if (j == 5) { // len
//         result[current->key] = current;
        result->insert(pair< Key, skipnode* >(current->key, current));
//         printf("in range: %i \n", current->key.value[5]->int_value);
      }
//       printf("found record: %s \n", current->record->payload);
      current = current->right[0];
    }
  } else { // can move to next dimension
    current = minpath[i];
    while (current != maxpath[i]) {
      findRecords(current->next_dim[i-1], minkey, maxkey, result);
      current = current->right[i];
    }
//     findRecords(minpath[i]->next_dim[i-1], minkey, maxkey);
    findRecords(maxpath[i]->next_dim[i-1], minkey, maxkey, result);
  }
  return kOk;
}

ErrorCode deleteRecord(skiplist* list, Key key) {
  int i;
  skipnode *path[NODEHEIGHT];
  skipnode *node;
  
  for (i = 0; i < NODEHEIGHT; i++) {
    path[i] = list->start;
  }
  
  node = findNodeAndTrack(list, key, path);
  node = node->right[0];
  if (node == 0)
    return kErrorNotFound;
  path[0]->right[0] = node->right[0]; 
//   for (i = 0; path[i] != list->start && i < NODEHEIGHT; i++) {
//     printf("o_O %s \n", path[i]->record->payload);
//   }
//   printf("o_O %s \n", node->record->payload);
  for (i = 1; i <= list->maxlevel; i++) {
    // if the linked list in level i jumped over the current node, we just delete it from next_dim,
    // otherwise we have to reconstruct next_dim from scratch
//     if (node->right[i] != 0) {
    if (path[i]->right[i] == node) {
      path[i]->right[i] = node->right[i];
      if (path[i]->next_dim[i-1] != 0) {
	deleteList(path[i]->next_dim[i-1]);
	path[i]->next_dim[i-1] = 0;
	fillNextDim(list, path[i], path[i]->right[i], i-1);
      } else {
// 	printf("o_O \n");
      }
    } else if (path[i]->next_dim[i-1] != 0) {
      deleteRecord(path[i]->next_dim[i-1], key);
    }
  }
//   printf("deleting node \n");
  deleteNode(node);
  return kOk;
//   printf("done \n");
}

ErrorCode insert(skiplist* list, Key key, Record* rec, skipnode **insnode) {
  int i;
  int newLevel;
  skipnode *node = createNode(key, rec);
  skipnode *position;
  skipnode *path[NODEHEIGHT];
  
//   if (attr.type != list->type) return kErrorIncompatibleKey;
  
  for (i = 0; i < NODEHEIGHT; i++) {
    path[i] = list->start;
  }
  
//   printf("finding pos, num_attribute=i \n"); //  list->num_attribute
  position = findNodeAndTrack(list, key, path);
//   printAttribute(attr, 1);
//   printf(" %i \n", position->right[2]);
//   if (position == list->start) printf("found start position! \n");
//   if (position->element == element) return 1;
//   printf("inserting %s on lowest level in lisdt %i\n", rec->payload, list->num);
  node->right[0] = position->right[0];
  position->right[0] = node;
  for (i = 1; i < NODEHEIGHT; i++) {
    if (randDouble() < list->p) {
//       printf("lifting up! \n");
      node->right[i] = path[i]->right[i];
      path[i]->right[i] = node;
      // doing the new nodes next_dim pointers
//       node->next_dim[i-1] = createList(list->type, list->num_attribute+1, list->attribute_count);
      if (i < list->maxlevel) {
	if (path[i]->next_dim[i-1] == 0) {
// 	  printf("deleting some NULL list \n");
	} else {
// 	  printf("deleting some list \n");
          deleteList(path[i]->next_dim[i-1]);
	}
	fillNextDim(list, node, node->right[i], i-1);
	fillNextDim(list, path[i], node, i-1);
// 	if (path[i] != list->start && path[i]->next_dim[i-1] != 0) {
// 	  insert(path[i]->next_dim[i-1], path[i]->key, path[i]->record, insnode);
// 	}
      }
    } else {
//       i++;
      break;
    }
  }
  // insert record into higher-level nodes
  for ( ; i < list->maxlevel; i++) {
    if (path[i]->next_dim[i-1] != 0) {
//       printf("inserting elemets %s in list %i \n", rec->payload, path[i]->next_dim[i-1]->num);
      insert(path[i]->next_dim[i-1], key, rec, insnode);
    } else {
//       printf("next_dim ist NULL ;_;, num_attr = %i, element = %s, node =  \n", list->num_attribute, rec->payload);
    }
  }
  if (i > list->maxlevel) {
//     printf("increasing level, %i \n", list->num_attribute);
    newLevel = i;
    for (i = list->maxlevel; i < newLevel; i++) {
//       printf("And creating new lists for this, i =%i \n", i);
      fillNextDim(list, list->start, node, i-1);
//       fillNextDim(list, list->start, 0, i-1);
      fillNextDim(list, node, 0, i-1);
//       list->start->next_dim[i-1] = createList(list->type, list->num_attribute+1, list->attribute_count);
//       printf("list->num = %i \n", list->start->next_dim[i-1]->num);
    }
    list->maxlevel = newLevel;
  }
//   printf("incrementing n \n");
  list->n++;
//   *insnode = node;
  
  return kOk;
}

void fillNextDim(skiplist *list, skipnode* owner, skipnode* limit, int height) {
  skipnode *current;// = owner->right[0];
  skipnode **insnode;
  
  if (owner == list->start) {
    current = owner->right[0];
  } else {
    current = owner;
  }
//   if (owner == 0)
//     return;
  
//   printf("filling some next_dim \n");
  owner->next_dim[height] = createList(list->type, list->num_attribute+1, list->attribute_count, list->max_dim);
//   printf("created list \n");
  if (owner->next_dim[height] == 0) // reached last dimension
    return;
  while (current != limit) {
//     printf("inserting %s in list %i \n", current->record->payload.data, list->num);
    insert(owner->next_dim[height], current->key, current->record, insnode);
    current = current->right[0];
  }
}

void printAllLists() {
  map< int, skiplist* >::iterator liter;

  for (liter = all_lists.begin(); liter != all_lists.end(); liter++) {
    if (liter->second != 0) {
      printList(liter->second);
    }
  }
}

// maybe want to use an iterator here?
void printList(skiplist* list) {
  int i;
  skipnode *current;// = list->start;
  
  printf("\nprinting list %i (num_attr = %i): \n", list->num, list->num_attribute);
  for (i = 0; i < list->maxlevel; i++) {
    current = list->start;
    while (current != 0) {
      if (current->right[list->maxlevel - i - 1] == 0) {
	printf("[x]");
      } else {
	printAttribute(*current->right[list->maxlevel - i - 1]->key.value[list->num_attribute], 1);
      }
      if (current->next_dim[list->maxlevel - i - 2] == 0) {
	  printf(",[o] \t");
	} else {
	  printf(",[%i] \t", current->next_dim[list->maxlevel - i - 2]->num);
	}
      current = current->right[0];
    }
    printf("\n");
  }
  printf(" .\t");
  current = list->start->right[0];
  while (current != 0) {
    printf("%s,", current->record->payload.data);
    printAttribute(*current->key.value[list->num_attribute], 0);
    printf("\t");
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
      if (l.int_value < r.int_value)     return -1;
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

// int compareKeyElements(Key left, Key right, int num_attribute) {
//   int i;
//   Attribute l = *left.value[num_attribute];
//   Attribute r = *right.value[num_attribute];
//   
//   return compareAttributes(l, r);
// }

int compareKeys(Key left, Key right, int num_attribute, bool elementOnly) {
  int i;
  Attribute l = *left.value[num_attribute];
  Attribute r = *right.value[num_attribute];
  int compare;
  
//   printf("comparing attributes: ");
//   printAttribute(l, 0);
//   printAttribute(r, 0);
//   printf("\n");
  compare = compareAttributes(l, r);
  
  if (!elementOnly) {
    if (compare == 0) {
      for (i = 0; i < left.attribute_count; i++) {
	if (i == num_attribute) continue;
	l = *left.value[i];
	r = *right.value[i];
	compare = compareAttributes(l, r);
	if (compare != 0) break;
      }
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
//   printf("\t");
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
