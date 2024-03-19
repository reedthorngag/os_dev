#include <stdbool.h>
#include <stdint.h>

#ifndef _TYPEDEFS
#define _TYPEDEFS

#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8  uint8_t

#define i64 int64_t
#define i32 int32_t
#define i16 int16_t
#define i8  int8_t

#define RGB(r,g,b) (u16)(((u8)(((float)b/0xff)*31) | ((u8)(((float)g/0xff)*31) << 5) | ((u8)(((float)r/0xff)*31) << 10)) & 0x7fff)

#define outb(port,value) asm volatile ("outb %0,%1" :: "a"((u8)(value)),"Nd"((u16)(port)))

#define inb(port,out) asm volatile ("inb %%dx,%%al" : "=a" (out) :"Nd"((u16)(port)))

#define _debug_str(str) for (u32 n = 0;str[n];) outb(0xe9,str[n++])

#define hcf() asm volatile ("cli"); while (true) asm volatile ("hlt")

#endif
