#include "skiplist.h"

int main(int argc, char *argv[]) {
  int i, j;
  int record_count = 4;
  uint8_t len = 5;
  AttributeType type[len];
  const char hans[] = "hans";
  Index *index;
  Transaction *t;
  Record recs[record_count];
  Key keys[record_count];
  Attribute attr;
  Block blocks[record_count];
  
  for (j = 0; j < record_count; j++) {
    blocks[j].size = 5;
    blocks[j].data = malloc(5);
    sprintf((char*)blocks[j].data, "%i", j);
    attr.type = kInt;
    keys[j].value = (Attribute**) malloc(len*sizeof(Attribute*));
    keys[j].attribute_count = len;
    printf("Key number %i: \n", j);
    for (i = 0; i < len; i++) {
      type[i] = kInt;
      keys[j].value[i] = (Attribute*) malloc(sizeof(Attribute));
      keys[j].value[i]->type = kInt;
      keys[j].value[i]->int_value = randInt(100);
      printf("key.value[%i] = %ld \n", i, keys[j].value[i]->int_value);
    }
    printf("\n");
    recs[j].key = keys[j];
    recs[j].payload = blocks[j];
  }
  
  CreateIndex(hans, len, type);
  OpenIndex(hans, &index);
  BeginTransaction(&t);
  for (i = 0; i < 4; i++) { //record_count
    InsertRecord(t, index, recs+i);
  }
  CommitTransaction(&t);
  CloseIndex(&index);
  DeleteIndex(hans);
//   printf("should be zero: %i \n", all_indices["hans"]);
}