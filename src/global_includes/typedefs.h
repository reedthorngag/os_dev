
#include <stdbool.h>
#include <stdint.h>


#define char unsigned char
#define word uint16_t
#define short uint16_t
#define int uint32_t
#define long uint64_t

#define RGB(r,g,b) (word)(((char)(((float)b/0xff)*31) | ((char)(((float)g/0xff)*31) << 5) | ((char)(((float)r/0xff)*31) << 10)) & 0x7fff)

#define hcf() asm volatile ("cli"); while (true) asm volatile ("hlt")

#define outb(port,value) asm volatile ("outb %0,%1" :: "a"((char)(value)),"Nd"((word)(port)))

#define inb(port,out) asm volatile ("inb %0,%1" : "=r" (out) :"Nd"((word)(port)))


