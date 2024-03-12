#include <typedefs.h>

void vmm_init();

void translate_vaddr_to_pmap(u64 virtual_address,u16 map[4]);

void map_pages(u64 vaddress, u64 paddress, u32 num_pages);


