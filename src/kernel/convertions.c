
#include <typedefs.h>
#include <convertions.h>

void long_to_hex(u64* value,u8* out) {
    for (u8 i=4,o=3<<4;i--;out+=4,o-=16)
        word_to_hex((u16)((*value>>o)&0xffff),out);
}

void int_to_hex(u32* value,u8* out) {
    word_to_hex((u16)((*value>>16)&0xffff),out);
    out += 4;
    word_to_hex((u16)(*value&0xffff),out);
}

void word_to_hex(u16 value,u8* out) {
    for (u8 i=4;i--;value>>=4)
        out[i] = "0123456789abcdef"[value&0xf];
}
