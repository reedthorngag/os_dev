#include <typedefs.h>
#include <debugging.h>

void panic(u32 code) {
    debug_("Kernel panic! error code: ",code);
    hcf();
}
