#ifndef _ROCKSDB_COMMON_
#define _ROCKSDB_COMMON_
const unsigned long OP_PUT   = 0;
const unsigned long OP_GET   = 1;
const unsigned long value_sz = 8;
typedef struct rock_kv_st{
  unsigned long op;
  unsigned long key;
  char value[value_sz];
}rock_kv_t;
#endif