
#include <typedefs.h>
#include <screen.h>
#include <debugging.h>
#include <vmm.h>
#include <pmm.h>

extern u16 screen_res_x;
extern u16 screen_res_y;
extern u32 screen_buffer_ptr_real;
extern u32 screen_buffer_size; // in 1KB blocks
extern u16 bytes_per_line;
extern u8  bytes_per_pixel;

extern u64 _binary_zap_vga16_psf_start;
extern u64 _binary_zap_vga16_psf_end;
extern u64 _binary_zap_vga16_psf_size;

u16* screen_buffer_ptr = (u16*)0x1000000;
u16 screen_default_background = RGB(255,0,0);


void screen_init() {

    map_p_range(screen_buffer_ptr_real,screen_buffer_size>>10,3);

    map_pages((u64)screen_buffer_ptr,screen_buffer_ptr_real&~0xfff,screen_buffer_size>>10);

    wipe_screen();
}

void wipe_screen() {
    u16* screen_buf = screen_buffer_ptr;

    u64 screen_buf_end = (u64)screen_buf + (u64)(screen_buffer_size<<2);
    for (;(u64)screen_buf<screen_buf_end;screen_buf++) {
        debug((u64)screen_buf);
        *screen_buf = screen_default_background;
    }
}

void draw_pixel(u16 x,u16 y,u16 color) {
    u16* pointer = screen_buffer_ptr;
    pointer += x;
    pointer += y*screen_res_x;
    *pointer = color;
}

void draw_rect(u16 x,u16 y, u16 width,u16 height, u16 color) {
    u16* pointer = screen_buffer_ptr;
    pointer += x;
    pointer += y*screen_res_x;

    for (;height--;pointer+=screen_res_x-width)
        for (u16 x=width;x--;pointer++)
            *pointer = color;
}


void write_string(u16 x,u16 y,char string[], u16 color, u16 background) {
    for (u32 i=0;string[i];i++){
        draw_glyph(x+i*8,y,string[i],color,background);}
}

void draw_glyph(u16 x,u16 y,char character,u16 color,u16 background) {
    u16* pointer = screen_buffer_ptr;
    pointer += x+1;
    pointer += y*screen_res_x;
    u8* char_ptr = ((u8*)&_binary_zap_vga16_psf_start)+3+character*16;

    for (u8 n=16;n--;) {
        decode_line(pointer,&char_ptr,color,background);
        pointer+=screen_res_x;
    }
}

void decode_line(u16* pointer,u8** char_ptr,u16 color,u16 background) {
    u8 line = *((*char_ptr)++);
    for (u8 n=8;n--;) {
        *(pointer++) = (line & 1<<n) ? color : background;
    }
}

