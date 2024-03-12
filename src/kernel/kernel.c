#include <typedefs.h>
#include <debugging.h>

#include <screen.h>
#include <convertions.h>
#include <vmm.h>

volatile void kernel_start() {

    vmm_init();
    
    screen_init();

    write_string(0,0,"Hello World!",RGB(255,255,255),RGB(0,0,0));

    hcf();
    
    return;
}
 

