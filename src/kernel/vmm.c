#include <typedefs.h>
#include <screen.h>
#include <convertions.h>
#include <exceptions.h>
#include <pmm.h>
#include <vmm.h>

#include <debugging.h>

u64* pml4 = (u64*)0x1000;
extern u64 pml3;
extern u64 pml2;
extern u64 pml1;

void vmm_init() {

    pmm_init();

    map_pages(0x400000,0,5);
    hcf();
}

u64 walk_pml_map(u64 vaddr) {

    u16 pml_map[4];
    translate_vaddr_to_pmap(vaddr,pml_map);

    
    return 0;
}

// translate virtual address to an array of pml 4-1 addresses, 4 is highest (index 4)
void translate_vaddr_to_pmap(u64 virtual_address, u16 pml_map[4]) {

    virtual_address>>=12; // divide virtual_address by 4096 (page size) to get the absolute page number

    for (u8 i=0;i<4;virtual_address>>=9,i++)
        pml_map[i] = (u16)(virtual_address&0x01ff);
    
    return;
}

void map_pages(u64 vaddress, u64 paddress, u32 num_pages) {
    debug("mapping pages...\n");

    u16 pml_map[4];
    translate_vaddr_to_pmap(vaddress,pml_map);

    desc_table:

    u64* pml_n = (u64*)0x1000;

    for (u8 level=4;--level;) {
        debug_("pml_n: ",(u64)pml_n);
        if (!pml_n[pml_map[level]]) {
            u64 pml_table = (u64)kmalloc(0x2000);
            if (!pml_table) {
                panic(-100);
            }
            pml_table = ((pml_table>>12)+1)<<12;
            pml_n[pml_map[level]] = pml_table | 3;
        }
        debug_("content: ",pml_n[pml_map[level]]);
        pml_n = (u64*)(pml_n[pml_map[level]]&~0xfff);
    }
    debug_("pml_n: ",(u64)pml_n);

    while (num_pages--) {
        *pml_n++ = paddress | 3;
        paddress += 0x1000;
        if (!((u64)pml_n&0xfff)) {
            pml_map[1]++;
            pml_map[0] = 0;
            goto desc_table;
        }
    }

    return;
}

