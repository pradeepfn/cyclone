#ifndef _PMEMKV_COMMON_
#define _PMEMKV_COMMON_
const unsigned long OP_PUT       = 0;
const unsigned long OP_GET       = 1;
const unsigned long OP_ADD       = 2;
const unsigned long value_sz     = 8;
typedef struct pmemkv_st{
  unsigned long op;
  unsigned long key;
  char value[value_sz];
}pmemkv_t;

static const char* KV_NAME = "cyclone_pmemkv";
static const char* KV_ENGINE = "kvtree";
static const int DB_SIZE_IN_GB = 1;
//unsigned long rocks_keys = 100000000;
unsigned long rocks_keys = 100;
#endif
