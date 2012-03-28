#include "naive_skiplist.h"

double randDouble() {
  return (double) rand() / (double) RAND_MAX;
}


skipnode* createNode(int element) {
  int i;
  skipnode *node = (skipnode*) malloc(sizeof(skipnode));
  
  for (i = 0; i < NODEHEIGHT; i++) {
    node->ptr[i] = 0;
  }
  node->element = element;
  
  return node;
}

skiplist* createList() {
  skiplist *list = (skiplist*) malloc(sizeof(skiplist));
  list->start = createNode(INT_MIN);
  list->p = 0.5;
  list->n = 0;
  list->maxlevel = 2;
  
  return list;
}

skipnode* findNode(skiplist *list, int element) {
  skipnode *path[NODEHEIGHT];
  
  return findNodeAndTrack(list, element, path);
}

skipnode* findNodeAndTrack(skiplist* list, int element, skipnode **path) {
//   int depth = 0;
  int level = list->maxlevel;
  skipnode *current = list->start;
  
  while (level > 0) {
    level--;
    while (current->ptr[level] != 0 && current->ptr[level]->element <= element) {
      current = current->ptr[level];
//       depth++;
    }
    path[level] = current;
  }
//   printf("found something with depth %i \n", depth);
  return current;
}

int insert(skiplist *list, int element) {
  int i;
  skipnode *node = createNode(element);
  skipnode *position;
  skipnode *path[NODEHEIGHT];
  
  for (i = 0; i < NODEHEIGHT; i++) {
    path[i] = list->start;
  }
  
  position = findNodeAndTrack(list, element, path);
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
  
  return 0;
}

// maybe want to use an iterator here?
void printtList(skiplist* list) {
  int i;
  skipnode *current;// = list->start;
  
  for (i = 0; i < list->maxlevel; i++) {
    current = list->start;
    while (current != 0) {
      if (current->ptr[list->maxlevel - i - 1] == 0) printf("[x]\t");
      else printf("[%i]\t", current->ptr[list->maxlevel - i - 1]->element);
      current = current->ptr[0];
    }
    printf("\n");
  }
  printf(" .\t");
  current = list->start->ptr[0];
  while (current != 0) {
    printf(" %i\t", current->element);
    current = current->ptr[0];
  }
  printf("\n");
}


int main(int argc, char *argv[]) {
  int i;
  skiplist *list = createList();
  skipnode *node;
  
  srand (time(NULL));
  
  insert(list, 5);
  insert(list, 35);
  insert(list, 17);
  insert(list, 20);
  insert(list, 2);
  insert(list, 28);
  insert(list, 56);
  insert(list, 25);
  insert(list, 12);
  insert(list, 33);
  insert(list, 87);
  insert(list, 92);
  insert(list, 15);
  insert(list, 65);
  insert(list, 42);
  insert(list, 37);
  insert(list, 11);
  insert(list, 5);
//   node = findNode(list, 5);
  
  printtList(list);
//   for (i = 0; i < NODEHEIGHT; i++) {
//     printf("ptr[%i] = %i \n", i, node->ptr[i]);
//   }
//   printf("value: %i \n", node->element);
  return 0;
}
