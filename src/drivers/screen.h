
#include <typedefs.h>

extern u16 screen_res_x;
extern u16 screen_res_y;
extern u32 screen_buffer_ptr_real;
extern u32 screen_buffer_size;
extern u32 virtual_scrn_buf_ptr;
extern u16 bytes_per_line;
extern u8 bytes_per_pixel;

extern u64 _binary_zap_vga16_psf_start;
extern u64 _binary_zap_vga16_psf_end;
extern u64 _binary_zap_vga16_psf_size;

extern u16* screen_buffer_ptr;

typedef enum u16 {
    Color_BLACK = RGB(0,0,0),
    Color_WHITE = RGB(255,255,255),
    Color_RED = RGB(255,0,0),
    Color_GREEN = RGB(0,255,0),
    Color_BLUE = RGB(0,0,255)
} Color;

void screen_init();

void wipe_screen();

void draw_pixel(u16 x,u16 y,u16 color);

void draw_rect(u16 x,u16 y, u16 width,u16 height, u16 color);

void write_string(u16 x,u16 y,char string[], u16 color, u16 background);

void draw_glyph(u16 x,u16 y,char character,u16 color,u16 background);

void decode_line(u16* pointer,u8** char_ptr,u16 color,u16 background);

void map_screen_buffer();

