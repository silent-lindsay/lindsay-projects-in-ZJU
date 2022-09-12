#include "printk.h"
#include "sbi.h"

extern void test();

int start_kernel() {
    printk("%d",2021);
    printk(" Hello RISC-V + 3190105609\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
