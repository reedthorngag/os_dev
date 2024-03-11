
#include <typedefs.h>
#include <debugging.h>
#include <convertions.h>

void long_to_hex(long* value,char* out) {
    for (char i=4,o=3<<4;i--;out+=4,o-=16)
        word_to_hex((word)((*value>>o)&0xffff),out);
}

void int_to_hex(int* value,char* out) {
    word_to_hex((word)((*value>>16)&0xffff),out);
    out += 4;
    word_to_hex((word)(*value&0xffff),out);
}

void word_to_hex(word value,char* out) {
    for (char i=4;i--;value=value>>4)
        out[i] = "0123456789abcdef"[value&0xf];
}

