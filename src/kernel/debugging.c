
#include <typedefs.h>
#include <convertions.h>

void debug_binary_b(char b) {
    for (char n=8;n--;)
        outb(0xe9,(b&(1<<n))>0?'1':'0');
    outb(0xe9,'\n');
    return;
}

void debug_short(short out) {
    char out_buf[4];
    word_to_hex(out,out_buf);
    for (char i=0;i<4;)
        outb(0xe9,out_buf[i++]);
    outb(0xe9,'\n');
    return;
}

void debug_long(long out) {
    char out_buf[16];
    for (char n=4;n--;out>>=16)
        word_to_hex(out,&out_buf[n<<2]);
    
    for (char i=0;i<16;)
        outb(0xe9,out_buf[i++]);
    outb(0xe9,'\n');
    return;
}
