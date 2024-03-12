#include <typedefs.h>

void pmm_init();

void create_physical_mem_map();

void map_p_range(u64 addr,u32 pages,u8 type);

u64 pmalloc(u32 pages,u8 type);

void pfree(u64 pageStart,u32 num);

void* kmalloc(u32 size);

void print_kmalloc_allocated_table();

void kfree(void* ptr);
