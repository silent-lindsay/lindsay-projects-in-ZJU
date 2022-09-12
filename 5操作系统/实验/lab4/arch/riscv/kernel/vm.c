// arch/riscv/kernel/vm.c
#include "vm.h"
#include "printk.h"

extern char _stext[];
extern char _etext[];
extern char _srodata[];
extern char _erodata[];
extern char _sdata[];
extern char _edata[];

/* early_pgtbl: ⽤于 setup_vm 进⾏ 1GB 的 映射。 */
unsigned long early_pgtbl[512] __attribute__((__aligned__(0x1000)));

/* 创建多级⻚表映射关系 */
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm);

void setup_vm(void)
{
    /*
    1. 由于是进⾏ 1GB 的映射 这⾥不需要使⽤多级⻚表
    2. 将 va 的 64bit 作为如下划分： | high bit | 9 bit | 30 bit |
    high bit 可以忽略
    中间9 bit 作为 early_pgtbl 的 index
    低 30 bit 作为 ⻚内偏移 这⾥注意到 30 = 9 + 9 + 12， 即我们只使⽤根⻚表， 根⻚表的每个 entry 都对应 1GB 的区
    域。
    3. Page Table Entry 的权限 V | R | W | X 位设置为 1
    */
    early_pgtbl[2] = (0x80000000 >> 2) + 15;
	early_pgtbl[384] = (0x80000000 >> 2) + 15;

    // //the first mapping
    // early_pgtbl[2] = 0x80000;   
    // early_pgtbl[2] = (uint64)(early_pgtbl[2] << 10);
    // early_pgtbl[2] = (uint64)(early_pgtbl[2] + 0xf);

    // //the secong mapping
    // early_pgtbl[384] = early_pgtbl[2];
    // return;
}

/* swapper_pg_dir: kernel pagetable 根⽬录， 在 setup_vm_final 进⾏映射。*/
unsigned long swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));

void setup_vm_final(void)
{
    memset(swapper_pg_dir, 0x0, PGSIZE);
    // // No OpenSBI mapping required

    // // mapping kernel text X|-|R|V
    // create_mapping(swapper_pg_dir,0xffffffe000200000,0x80200000,0x2000,11);
    // // mapping kernel rodata -|-|R|V
    // create_mapping(swapper_pg_dir,0xffffffe000202000,0x80202000,0x1000,3);
    // // mapping other memory -|W|R|V
    // create_mapping(swapper_pg_dir,0xffffffe000203000,0x80203000,0x7dfd000,7);

    // mapping kernel text X|-|R|V
    create_mapping(swapper_pg_dir, _stext , _stext - PA2VA_OFFSET, _etext - _stext, 11);
    // mapping kernel rodata -|-|R|V
    create_mapping(swapper_pg_dir, _srodata , _srodata - PA2VA_OFFSET, _erodata - _srodata, 3);    
    // mapping other memory -|W|R|V
    create_mapping(swapper_pg_dir, _sdata , _sdata - PA2VA_OFFSET, PHY_END + PA2VA_OFFSET - (uint64)_sdata, 7);
    // set satp with swapper_pg_dir
    uint64 csrstap = (((uint64)swapper_pg_dir - PA2VA_OFFSET) >> 12) + 0x8000000000000000;
    asm volatile("csrw satp, %0" :: "r"(csrstap));
    // YOUR CODE HERE
    // flush TLB
    asm volatile("sfence.vma zero, zero");

    // printk("create mapping is ok!\n") ;
    return;
}
/* 创建多级⻚表映射关系 */
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm)
{
    /*
    pgtbl 为根⻚表的基地址
    va, pa 为需要映射的虚拟地址、物理地址
    sz 为映射的⼤⼩
    perm 为映射的读写权限，可设置不同section所在⻚的属性，完成对不同section的保护
    创建多级⻚表的时候可以使⽤ kalloc() 来获取⼀⻚作为⻚表⽬录
    可以使⽤ V bit 来判断⻚表项是否存在
    */

    uint64 va_i, vpn0, vpn1, vpn2;
    int PG_NUM = (sz+4095)/4096 ; // ceil the page number
    for ( int i = 0 ; i < PG_NUM ; i++ ) 
    {
        va_i = va + (i << 12);
        vpn2 = (va_i << 25) >> 55;
		vpn1 = (va_i << 34) >> 55;
		vpn0 = (va_i << 43) >> 55;

        if(pgtbl[vpn2] % 2 == 0)
        {
            uint64 tmp = kalloc() - PA2VA_OFFSET;
            pgtbl[vpn2] = ((tmp >> 12) << 10) + 1;
        }

        uint64 *pgt1 = (uint64 *)((pgtbl[vpn2] >> 10) << 12);
		
		if (pgt1[vpn1] % 2 == 0)
        {											  
			uint64 tmp = kalloc() - PA2VA_OFFSET;
			pgt1[vpn1] = ((tmp >> 12) << 10) + 1;
		}
		uint64 *pgt0 = (uint64 *)(((pgt1[vpn1] >> 10) << 12)+PA2VA_OFFSET);

		pgt0[vpn0] = (((pa >> 12) + i) << 10) + (perm);
    }

    // int vpn2, vpn1, vpn0;
    // for( uint64 i = 0; i<sz ; i = i + 0x1000 ){

    //     vpn2 = (int)((va & 0x7fc0000000 ) >> 30);
    //     vpn1 = (int)((va & 0x3fe00000) >> 21);
    //     vpn0 = (int)((va & 0x1f000)>>12);//line5

    //     if( (pgtbl[vpn2] & 0x0000000000000001) != 0x1 ) {
    //        uint64 add = kalloc() - PA2VA_OFFSET;
    //        add = (uint64)((uint64)(add>>2) + 0x1);
    //        pgtbl[vpn2] = add;
    //     }

    //     uint64* pgtbl2= (uint64*)((uint64)((uint64)pgtbl[vpn2] >> 10) << 12);
    //     if( (pgtbl2[vpn1] & 0x0000000000000001) != 0x1) {
    //        uint64 add = kalloc() - PA2VA_OFFSET;
    //        add = (uint64)((uint64)(add>>2) + 0x1);
    //        pgtbl2[vpn1] = add;
    //     }
    //     uint64* pgtbl3= (uint64*) ( ( pgtbl2[vpn1] >> 10) << 12 + PA2VA_OFFSET ) ;	
    //     pgtbl3[vpn0] = (uint64)((uint64)((pa >>12) << 10) + (uint64)perm);
    //     pa += 0x1000 ;
    //     va += 0x1000 ;      
    // }
}