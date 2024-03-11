#include <typedefs.h>

#include <paging.h>
#include <screen.h>
#include <convertions.h>
#include <paging.h>

volatile void kernel_start() {

    int a = 0x7692; // 0x0410 

    screen_init(); // 0x2120

    //paging_init();

    char* hello = (char*)"Hello World!";
    write_string(0,0,hello,RGB(100,100,100),RGB(0,0,0));

    hcf();
    
    return; 
}
 

