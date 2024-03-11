
#include <typedefs.h>
#include <screen.h>

extern word screen_res_x;
extern word screen_res_y;
extern int screen_buffer_ptr_real;
extern int screen_buffer_size;
extern int virtual_scrn_buf_ptr;
extern word bytes_per_line;
extern char bytes_per_pixel;

extern long _binary_zap_vga16_psf_start;
extern long _binary_zap_vga16_psf_end;
extern long _binary_zap_vga16_psf_size;

word* screen_buffer_ptr;
word screen_default_background = RGB(0,0,0);


void map_screen_buffer() {
    
    int scrn_buf_virtual_address = (screen_buffer_ptr_real - screen_buffer_ptr_real % 0x1000) | 3;

    int* pdbt = (int*)0x3008;
    *pdbt = (int)0x5003;

    int* page_file_end = (int*)0x5000;
    int virtual_address = (0x200000 + screen_buffer_ptr_real % 0x1000);

    for (int i=0; i<(screen_buffer_size>>7);i++,scrn_buf_virtual_address+=0x1000,page_file_end+=2) {
        *page_file_end = scrn_buf_virtual_address;
    }

    virtual_scrn_buf_ptr = virtual_address;
    return;
}

void screen_init() {
    map_screen_buffer();
    screen_buffer_ptr = (word*)(long)virtual_scrn_buf_ptr;
    wipe_screen();
    hcf();
    draw_pixel(0,0,rgb(255,0,0));
    hcf();
}

void wipe_screen() {
    word* screen_buf = screen_buffer_ptr;

    long screen_buf_end = (long)screen_buf + (long)(screen_buffer_size<<5);

    for (int count=0;(long)screen_buf<screen_buf_end;screen_buf++,count++) {
        *screen_buf = screen_default_background;
    }
}

void draw_pixel(word x,word y,word color) {
    word* pointer = screen_buffer_ptr;
    pointer += x;
    pointer += y*screen_res_x;
    *pointer = color;
}

void draw_rect(word x,word y, word width,word height, word color) {
    word* pointer = screen_buffer_ptr;
    pointer += x;
    pointer += y*screen_res_x;

    for (;height--;pointer+=screen_res_x-width)
        for (word x=width;x--;pointer++)
            *pointer = color;
}


void write_string(word x,word y,char* string, word color, word background) {
    for (int i=0;string[i];i++){
        draw_glyph(x+i*8,y,string[i],color,background);}
}

void draw_glyph(word x,word y,char character,word color,word background) {
    word* pointer = screen_buffer_ptr;
    pointer += x+1;
    pointer += y*screen_res_x;
    char* char_ptr = ((char*)&_binary_zap_vga16_psf_start)+3+character*16;

    for (char n=16;n--;) {
        decode_line(pointer,&char_ptr,color,background);
        pointer+=screen_res_x;
    }
}

void decode_line(word* pointer,char** char_ptr,word color,word background) {
    char line = *((*char_ptr)++);
    for (char n=8;n--;) {
        *(pointer++) = (line & 1<<n) ? color : background;
    }
}

