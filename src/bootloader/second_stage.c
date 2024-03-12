#include "second_stage.h"

u64* pml4_tmp = (u64*)0x1000;
u64 pml_space_addr;
u64* pml_space_ptr;
u64 kernel_start = 0xfff7000000;
extern u64 pml_space_start;
extern u64 pml_space_end;
extern u64 screen_res_x;
extern u64 physical_kernel_start;

void main() {

    hcf();

    pml_space_addr = 0x80000;
    pml_space_ptr = (u64*)(&physical_kernel_start+pml_space_addr);
    pml_space_end = kernel_start+pml_space_addr;

    physical_kernel_start = (u64)&physical_kernel_start;

    map_kernel(kernel_start,(u64)&physical_kernel_start,0x2000);

    map_kernel(0x400000,0x10000,3);

    pml_space_start = kernel_start+pml_space_addr;

    u64* from = &screen_res_x;
    u64* to = (u64*)kernel_start;
    for (u32 i=0;i<(0x400/sizeof(u64));i++)
        *to++ = *from++;

    goto *(void*)0xfff7000400;

}

#include <convertions.c>
#include <debugging.c>

void* alloc_page() {
    pml_space_addr -= 0x1000;
    pml_space_ptr -= 0x200;
    return pml_space_ptr;
}

// translate virtual address to an array of pml 4-1 addresses, 4 is highest (index 4)
void vaddr_to_pmap(u64 virtual_address,u16 pml_map[4]) {

    virtual_address>>=12; // divide virtual_address by 4096 (page size) to get the absolute page number

    for (u8 i=0;i<4;virtual_address>>=9,i++)
        pml_map[i] = (u16)(virtual_address&0x01ff);
    
    return;
}

void map_kernel(u64 vaddress, u64 paddress, u32 num_pages) {
    u16 pml_map[4];
    vaddr_to_pmap(vaddress,pml_map);

    desc_table:

    u64* pml_n = pml4_tmp;

    for (u8 level=4;--level;) {
        if (!pml_n[pml_map[level]]) {
            u64 pml_table = (u64)alloc_page();

            pml_table = ((pml_table>>12)+1)<<12;
            pml_n[pml_map[level]] = pml_table | 3;
        }
        pml_n = (u64*)(pml_n[pml_map[level]]&~0xfff);
    }

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

