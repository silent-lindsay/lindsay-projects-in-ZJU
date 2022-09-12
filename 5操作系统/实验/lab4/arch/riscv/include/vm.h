#include "mm.h"
#include "defs.h"
#include "string.h"

void setup_vm(void);
void setup_vm_final(void);
void create_mapping(uint64* pgtbl,uint64 va, uint64 pa, uint64 sz, int perm);
