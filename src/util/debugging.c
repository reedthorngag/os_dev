
#include <typedefs.h>
#include <debugging.h>

void debug_bool(bool out) {
    if (out)
        debug_str("true");
    else
        debug_str("false");
}

void debug_u8(u8 b) {
    for (u8 n=8;n--;)
        outb(0xe9,(b&(1<<n))>0?'1':'0');
    outb(0xe9,'\n');
    return;
}

void debug_u16(u16 out) {
    char out_buf[4];
    word_to_hex(out,(u8*)&out_buf[0]);
    for (u8 i=0;i<4;)
        outb(0xe9,out_buf[i++]);
    outb(0xe9,'\n');
    return;
}

void debug_u32(u32 out) {
    char out_buf[8];
    word_to_hex((u16)(out>>16),(u8*)&out_buf[0]);
    word_to_hex((u16)out,(u8*)&out_buf[4]);
    for (u8 i=0;i<8;)
        outb(0xe9,out_buf[i++]);
    outb(0xe9,'\n');
    return;
}

void debug_u64(u64 out) {
    char out_buf[16];
    for (u8 n=4;n--;out>>=16)
        word_to_hex(out,(u8*)&out_buf[n<<2]);
    
    for (u8 i=0;i<16;)
        outb(0xe9,out_buf[i++]);
    outb(0xe9,'\n');
    return;
}

void debug_str(char str[]) {
    for (;*str;) outb(0xe9,*str++);
}
