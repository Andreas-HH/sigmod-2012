#include "kd_skiplist.h"

unordered_map< string, Index* > all_indices;

void printIndex(Index *index);

void printIndex(Index* index) {
  int i;
  
//   printList(index->list);
  printAllLists();
  
//   for (i = 0; i < index->attribute_count; i++) {
//     printList(index->lists[i]);
//     printf("\n");
//   }
}

//////////////////////////////////////////////////////////////
//    implementation of contest_interface methods begins    //
//////////////////////////////////////////////////////////////

ErrorCode BeginTransaction(Transaction **tx) {
  Transaction *t = (Transaction*) malloc(sizeof(Transaction));
  t->deleted_nodes = new set< skipnode* >();
  t->inserted_nodes = new set< skipnode* >();
  t->updates = new map< skipnode*, Block* >();
//   t->deleted_nodes->insert(0);
  
  *tx = t;
  return kOk;
}

ErrorCode AbortTransaction(Transaction **tx) {
  return kOk;
}

ErrorCode CommitTransaction(Transaction **tx) {
  return kOk;
}

ErrorCode CreateIndex(const char* name, uint8_t attribute_count, KeyType type) {
  int i;
  Index *index;
  time_t t = time(NULL);
  
  printf("using seed: %ld \n", t);
  srand (1334618018); // time(NULL)
  if (all_indices[string(name)] != 0) 
    return kErrorIndexExists;
  index = (Index*) malloc(sizeof(Index));
  if (index == 0) 
    return kErrorOutOfMemory;
  if (attribute_count == 0)
    return kErrorGenericFailure;
  
  index->name = new string(name);
  index->attribute_count = attribute_count;
//   index->lists = (skiplist**) malloc(attribute_count*sizeof(skiplist*));
  
  index->list = createList(type, 0, attribute_count, 3);
//   for (i = 0; i < attribute_count; i++) {
//     index->lists[i] = createList(type, i, attribute_count);
//     printtList(index->lists[i]);
//   }
  
  all_indices[string(name)] = index;
  return kOk;
}

ErrorCode OpenIndex(const char* name, Index **idx) {
  *idx = all_indices[string(name)];
  return kOk;
}

ErrorCode CloseIndex(Index **idx) {
//   printIndex(*idx);
  return kOk;
}

ErrorCode DeleteIndex(const char* name) {
  int i;
  Index *index = all_indices[string(name)];
  ErrorCode err;
  
  printf("deleting an index with %i keys \n", index->attribute_count);
  if (index == 0)
    return kErrorUnknownIndex;
//   for (i = 0; i < index->attribute_count; i++) {
//     err = deleteList(index->lists[i]);
//     if (err != kOk) // we don't know if some memroy wasn't freed here
//       return err;
//   }
  deleteList(index->list);
  
  return kOk;
}

ErrorCode InsertRecord(Transaction *tx, Index *idx, Record *record) {
  skipnode *node;
  
//   printf("Inserting some record \n");
//   int listnum = randInt(idx->attribute_count);
//   printf("taking list #%i \n", listnum);
//   printtList(idx->lists[listnum]);
  insert(idx->list, record->key, record, &node);
  tx->inserted_nodes->insert(node);
//   printtList(idx->lists[listnum]);
//   printIndex(idx);
  return kOk;
}

ErrorCode UpdateRecord(Transaction *tx, Index *idx, Record *record, Block *new_payload, uint8_t flags) {
  return kOk;
}

ErrorCode DeleteRecord(Transaction *tx, Index *idx, Record *record, uint8_t flags) {
  return deleteRecord(idx->list, record->key);
}

ErrorCode GetRecords(Transaction *tx, Index *idx, Key min_keys, Key max_keys, Iterator **it) {
  return findRecords(idx->list, min_keys, max_keys);
}

ErrorCode GetNext(Iterator *it, Record** record) {
  return kOk;
}

ErrorCode CloseIterator(Iterator **it) {
  return kOk;
}
