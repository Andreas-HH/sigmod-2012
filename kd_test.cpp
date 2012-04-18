#include "skiplist.h"

int main(int argc, char *argv[]) {
  int i, j;
  int record_count = 400;
  int range = record_count/10;
  uint8_t len = 5;
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
    keys[j].attribute_count = len;
//     printf("Key number %i: \n", j);
    for (i = 0; i < len; i++) {
      type[i] = kInt;
      keys[j].value[i] = (Attribute*) malloc(sizeof(Attribute));
      keys[j].value[i]->type = kInt;
      keys[j].value[i]->int_value = randInt(range);
//       printf("key.value[%i] = %ld \n", i, keys[j].value[i]->int_value);
    }
//     printf("\n");
    recs[j].key = keys[j];
    recs[j].payload = blocks[j];
  }
  // generate min and max keys
  minkey.value = (Attribute**) malloc(len*sizeof(Attribute*));
  minkey.attribute_count = len;
  maxkey.value = (Attribute**) malloc(len*sizeof(Attribute*));
  maxkey.attribute_count = len;
  for (i = 0; i < len; i++) {
    minkey.value[i] = (Attribute*) malloc(sizeof(Attribute));
    minkey.value[i]->type = kInt;
    minkey.value[i]->int_value = 0.3*range;
    maxkey.value[i] = (Attribute*) malloc(sizeof(Attribute));
    maxkey.value[i]->type = kInt;
    maxkey.value[i]->int_value = 0.8*range;
  }
  printf("Done with generating data. \n");
  
  tm = time(NULL);
  CreateIndex(hans, len, type); // len
  OpenIndex(hans, &index);
  BeginTransaction(&t);
  for (i = 0; i < record_count; i++) { //record_count
    InsertRecord(t, index, recs+i);
    if (i % 1000 == 0) 
      printf("i = %i / %i \n", i, record_count);
  }
  printf("finished inserting, took %is \n", time(NULL)-tm);
  tm = time(NULL);
//   for (i = 0; i < record_count; i+=2) { // record_count
//     if (i == 398) {
//       printf("deleting record %i \n", i);
//     }
    DeleteRecord(t, index, recs+4, 0);
//   }
//   if (DeleteRecord(t, index, recs+398, 0) != kErrorNotFound) {
//     printf(";_; \n ");
//   }
  printf("deleted half of the records, took %is \n", time(NULL)-tm);
  GetRecords(t, index, minkey, maxkey, iterator);
  while (GetNext(*iterator, &currentResult) != kErrorNotFound) {
    printf("%i \t(%i\t%i\t%i\t%i\t%i)\t%s \n", currentResult->key.value[0]->int_value, currentResult->key.value[0]->int_value, currentResult->key.value[1]->int_value, currentResult->key.value[2]->int_value, currentResult->key.value[3]->int_value, currentResult->key.value[4]->int_value, currentResult->payload.data);
  }
  printf("just to be sure: \n");
  for (i = 0; i < record_count; i++) {
    for (j = 0; j < len; j++) { // len
//       if (!(compareKeys(minkey, keys[i], j, true) <= 0 && compareKeys(keys[i], maxkey, j, true) < 0)) {
	if (!((minkey.value[j] == 0 || compareKeys(minkey, keys[i], j, true) <= 0) && (maxkey.value[j] == 0 || compareKeys(keys[i], maxkey, j, true) < 0))) {
	break;
      }
    }
    if (j == len) { // len
      printf("so: %i \n", keys[i].value[0]->int_value);
    }
  }
//   printf("committing \n");
  CommitTransaction(&t);
//   printf("closing \n");
  CloseIndex(&index);
//   printf("deleting \n");
  DeleteIndex(hans);
//   printf("should be zero: %i \n", all_indices["hans"]);
  return 0;
}