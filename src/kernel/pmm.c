#include <pmm.h>
#include <vmm.h>
#include <typedefs.h>
#include <debugging.h>

extern u8 kernel_start;
extern u8 kernel_end;
extern u64 pml_space_start;
extern u64 pml_space_end;

extern u64 physical_kernel_start;

extern u32 screen_buffer_ptr_real;

u8* kmalloc_table;
void* kmalloc_data;
const u16 kmalloc_blk_size = 128;
u32 kmalloc_table_size;

u8* nibble_map_ptr; // physical memory map start ptr
u64 nibble_map_size; // in bytes

extern u64 mem_map_buffer;
extern u16 mem_map_size;

// nibble map key:
// type 0: free
// type 1: allocated mem
// type 2: kernel mem
// type 3: mem mapped io and screen mem
// type 4-14: undefined
// type 15: unusable mem/mem holes

struct mem_map_ent {
    u64 addr;
    u64 size;
    u32 type;
};

void pmm_init() {

    u64 abs_size = (u64)(screen_buffer_ptr_real&~0xfff)-(u64)pml_space_end;
    kmalloc_table_size = (u32)(abs_size>>(kmalloc_blk_size>>5));

    kmalloc_table = (u8*)pml_space_end;
    kmalloc_data = (void*)(pml_space_end+kmalloc_table_size);

    create_physical_mem_map();

    u64 _kmalloc_data = pmalloc(1000,2); // 4MB
    map_pages(_kmalloc_data,_kmalloc_data,1000);

    kmalloc_table_size = _kmalloc_data>>(kmalloc_blk_size>>5);

    u64 _kmalloc_table = pmalloc((kmalloc_table_size>>12)+1,2);
    map_pages(_kmalloc_table,_kmalloc_table,(kmalloc_table_size>>12)+1);

    kmalloc_data = (void*)_kmalloc_data;
    kmalloc_table = (u8*)_kmalloc_table;

    return;
}

void create_physical_mem_map() {
    struct mem_map_ent* map = (struct mem_map_ent*)&mem_map_buffer;

    u64 highestaddr = 0;
    u64 highestsize = 0;
    u64 largestaddr = 0;
    u64 largestsize = 0;

    for (u32 i=0;i<mem_map_size;i++) {
        debug("\nmem_ent:\n");
        debug_("\taddr: ",map[i].addr);
        debug_("\tsize: ",map[i].size);
        debug_("\ttype: ",map[i].type);

        if (map[i].type != 1 && map[i].type != 3) continue;

        if (map[i].size > largestsize) {
            largestaddr = map[i].addr;
            largestsize = map[i].size;
        }

        if (map[i].addr > highestaddr) {
            highestaddr = map[i].addr;
            highestsize = map[i].size;
        }
    }

    u64 size = highestaddr + highestsize;

    debug_("\nmemory size: ",size);
    debug_("addr: ",highestaddr);
    debug_("size: ",highestsize);

    nibble_map_size = (size >> 13) + 1; // divide by 4096 (= one page per byte) then by 2 (= 2 pages per byte / 1 per nibble), add one for odd numbers of pages

    nibble_map_ptr = (u8*)largestaddr;

    debug_("nibble_map_start:",(u64)nibble_map_ptr);
    debug_("size: ",nibble_map_size);
    map_pages((u64)nibble_map_ptr, (u64)nibble_map_ptr, (nibble_map_size>>12)+((nibble_map_size&0xfff)>0));
    hcf();

    u64* ptr = (u64*)nibble_map_ptr;
    for (u64 target = (u64)ptr + nibble_map_size; (u64)++ptr < target;) {
        *ptr = ~0;
    }
    *(u64*)(nibble_map_ptr+nibble_map_size-8) = ~0;

    for (u32 i=0;i<mem_map_size;i++) {
        struct mem_map_ent ent = map[i];
        if (ent.type != 1 && ent.type != 3) continue;
        map_p_range(ent.addr&(~0xfff),(ent.size>>12),0); // assume (exceds the bounds if not) page aligned, this could/will break on real hardware
    }

    // map kernel
    map_p_range(physical_kernel_start&~0x1fff,(((u64)kmalloc_table-(u64)&kernel_start)>>12),2);
}

void map_p_range(u64 addr,u32 pages,u8 type) {
    u32 rel_addr = (addr>>12) + pages;

    u8* ptr = nibble_map_ptr + (rel_addr>>1);

    for (;pages--;rel_addr--) {
        if (rel_addr&1) {
            *ptr &= (u8)0x0f;
            *ptr-- |= (u8)(type<<4);
        } else {
            *ptr &= (u8)0xf0;
            *ptr |= type;
        }
    }
}

u64 pmalloc(u32 pages,u8 type) {
    u8* ptr = nibble_map_ptr;
    for (u32 size=nibble_map_size<<1;size--;) {

        ptr += (1-(size&1));
        if (*ptr>0xf*(size&1)) continue;

        u32 n=pages;
        for (;--n && size; size--) {
            if (*ptr > 0xf*(size&1)) break;
            ptr += size&1;
        }

        if (n) continue;

        for (;pages--;size++) {
            *ptr |= (u8)(type<<(4*(size&1)));
            ptr-= (1-(size&1));
        }

        debug_("ptr: ",(u64)ptr);
        
        return ((((u64)ptr-(u64)nibble_map_ptr)<<1)+(size&1))<<12;
    }

    return ~0;
}

void pfree(u64 start_addr,u32 num) {
    u32 rel_addr = (u32)(start_addr>>12) + num;

    u8* ptr = nibble_map_ptr + (rel_addr>>1);

    for (;num--;rel_addr--) {
        debug_("ptr: ",(u64)ptr);
        *ptr &= (u8)(0xf<<(4*(1-(rel_addr&1))));
        ptr-= (1-(rel_addr&1));
    }

}

void* kmalloc(u32 size) {
    // remember to fix equivelent problems in kfree() if fixing stuff

    size += 4;

    u32 large_blks = size >> 10; // TODO: calculate bitshift based on kmalloc_blk_size
    u32 blks = ((size & (kmalloc_blk_size*8-1)) >> 7) + 1; // TODO: check not exactly equal before adding 1

    if (large_blks) {
        u8 find = ((1<<blks)-1)<<(8-blks);
        u8* kmalloc_ptr = kmalloc_table;
        for (;(u64)kmalloc_ptr!=(u64)kmalloc_data;kmalloc_ptr++) { // loop through the kmalloc table
            
            if (*kmalloc_ptr) // block not free
                continue;
            
            u32 b = (u32)(kmalloc_ptr-kmalloc_table);
            for (u32 j=large_blks;--j;) { // check enough are free
                if (*++kmalloc_ptr)
                    break;
            }
            if (find & *++kmalloc_ptr) // block part not free
                continue;
            
            // check still smaller than the end of the table
            if ((u64)kmalloc_ptr>=(u64)kmalloc_data)
                return 0;
            
            *kmalloc_ptr |= find; // set block part taken
            for (u32 k=large_blks;k--;) // fill taken blocks
                *--kmalloc_ptr = 0xff;
            
            u32* data = (u32*)(kmalloc_data+(b<<10)+4);
            *data = large_blks*8+blks;

            return (void*)(++data);
        }
    } else {
        u8 find = ((1<<blks)-1);
        u8* kmalloc_ptr = kmalloc_table;
        for (;(u64)kmalloc_ptr!=(u64)kmalloc_data;kmalloc_ptr++) {
            
            if (find & *kmalloc_ptr)
                continue;

            u32 j=0;
            for (u8 c=*kmalloc_ptr; j<9-blks && !(find<<++j & c););
            *kmalloc_ptr |= find << --j;
            u32* data = (u32*)(kmalloc_data+((u32)(kmalloc_ptr-kmalloc_table)<<10)+((8-(j+blks))<<7));
            *data = blks;
            return (void*)(++data);
        }
    }

    // TODO: do some fancy paging shit to extend the kmalloc table/space when (nearly, or we are fucked anyway) full
    return 0;
}

void kfree(void* ptr) {

    u32 size = *(u32*)(ptr-sizeof(u32));
    u32 large_blks = size >> 10;
    u32 blks = ((size & (kmalloc_blk_size*8-1)) >> 7) + 1;

    if (large_blks) {
        return;
        u8 bits = ((1<<blks)-1)<<(8-blks);
        u8* kmalloc_ptr = kmalloc_table + ((ptr-kmalloc_data)>>10);
        for (;large_blks--;)
            *kmalloc_ptr++ = 0;
        *kmalloc_ptr ^= bits;
    } else {
        u8* kmalloc_ptr = kmalloc_table + ((ptr-kmalloc_data)>>10);
        u8 bits = ((1<<blks)-1) << (8-((ptr-kmalloc_data)>>7 & 0b111)-blks);
        *kmalloc_ptr ^= bits;
    }
}

void print_kmalloc_allocated_table() {
    debug("kmalloc table:\n");
    for (u32 i=0;i<kmalloc_table_size;i++) {
        if (!kmalloc_table[i]) {
            debug_binary(0);
            return;
        }
        debug_binary(kmalloc_table[i]);
    }
}



