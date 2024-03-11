#include <typedefs.h>
#include <screen.h>
#include <paging.h>
#include <convertions.h>

#include <debugging.h>

void paging_init() {
    word pml_map[4] = {0};
    translate_vaddr_to_pmap(0xffff80000000,pml_map);
    char buff[5] = {0};

    for (char i=4;i--;) {
        word_to_hex(pml_map[i],buff);
        write_string(0,(i+1)*16,buff,Color_WHITE,Color_BLACK);
    }
}

// translate virtual address to an array of pml 1-4 addresses
void translate_vaddr_to_pmap(long virtual_address,word pml_map[4]) {

    virtual_address>>=12; // divide virtual_address by 4096 to get the absolute page number

    for (char i=0;i<4;virtual_address>>=9,i++)
        pml_map[i] = (short)(virtual_address&0x01ff);
    
    return;
}

void map_section() {

}

void map_page() {
    
}

