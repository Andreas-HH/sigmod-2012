#include "skiplist.h"

int main(int argc, char *argv[]) {
  int i, j;
  int record_count = 10000;
  uint8_t len = 6;
  AttributeType type[len];
  const char hans[] = "hans";
  Index *index;
  Iterator **iterator = (Iterator**) malloc(sizeof(Iterator*));
  Transaction *t;
  Record recs[record_count];
  Key keys[record_count];
  Key minkey;
  Key maxkey;
  Record *currentResult;
  Attribute attr;
  Block blocks[record_count];
  time_t tm;
  
  for (j = 0; j < record_count; j++) {
    blocks[j].size = 5;
    blocks[j].data = malloc(5);
    sprintf((char*)blocks[j].data, "%i", j);
    attr.type = kInt;
    keys[j].value = (Attribute**) malloc(len*sizeof(Attribute*));
    keys[j].attribute_count = 5;
//     printf("Key number %i: \n", j);
    for (i = 0; i < 5; i++) {
      type[i] = kInt;
      keys[j].value[i] = (Attribute*) malloc(sizeof(Attribute));
      keys[j].value[i]->type = kInt;
      keys[j].value[i]->int_value = randInt(record_count/100);
//       printf("key.value[%i] = %ld \n", i, keys[j].value[i]->int_value);
    }
    keys[j].value[i] = (Attribute*) malloc(sizeof(Attribute));
    keys[j].value[i]->type = kInt;
    keys[j].value[i]->int_value = j;
//     printf("\n");
    recs[j].key = keys[j];
    recs[j].payload = blocks[j];
  }
  // generate min and max keys
  minkey.value = (Attribute**) malloc(len*sizeof(Attribute*));
  minkey.attribute_count = 5;
  maxkey.value = (Attribute**) malloc(len*sizeof(Attribute*));
  maxkey.attribute_count = 5;
  for (i = 0; i < len; i++) {
    minkey.value[i] = (Attribute*) malloc(sizeof(Attribute));
    minkey.value[i]->type = kInt;
    minkey.value[i]->int_value = 30;
    maxkey.value[i] = (Attribute*) malloc(sizeof(Attribute));
    maxkey.value[i]->type = kInt;
    maxkey.value[i]->int_value = 60;
  }
  printf("Done wit hgenerating data. \n");
  
  tm = time(NULL);
  CreateIndex(hans, 5, type); // len
  OpenIndex(hans, &index);
  BeginTransaction(&t);
  for (i = 0; i < record_count; i++) { //record_count
    InsertRecord(t, index, recs+i);
    if (i % 10000 == 0) 
      printf("i = %i / %i \n", i, record_count);
  }
  printf("finished inserting, took %is \n", time(NULL)-tm);
  tm = time(NULL);
  for (i = 0; i < record_count; i+=2) {
//     DeleteRecord(t, index, recs+i, 0);
  }
//   DeleteRecord(t, index, recs, 0);
  printf("deleted half of the records, took %is \n", time(NULL)-tm);
  GetRecords(t, index, minkey, maxkey, iterator);
  while (GetNext(*iterator, &currentResult) != kErrorNotFound) {
    printf("%i \t(%i\t%i\t%i\t%i\t%i) \n", currentResult->key.value[5]->int_value, currentResult->key.value[0]->int_value, currentResult->key.value[1]->int_value, currentResult->key.value[2]->int_value, currentResult->key.value[3]->int_value, currentResult->key.value[4]->int_value);
  }
  printf("just to be sure: \n");
  for (i = 0; i < record_count; i++) {
    for (j = 0; j < 5; j++) { // len
      if (!(compareKeys(minkey, keys[i], j, true) <= 0 && compareKeys(keys[i], maxkey, j, true) < 0)) {
	break;
      }
    }
    if (j == 5) { // len
      printf("so: %i \n", keys[i].value[5]->int_value);
    }
  }
  CommitTransaction(&t);
  CloseIndex(&index);
  DeleteIndex(hans);
//   printf("should be zero: %i \n", all_indices["hans"]);
  return 0;
}