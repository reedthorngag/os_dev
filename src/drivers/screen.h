
#include <typedefs.h>

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

extern word* screen_buffer_ptr;

typedef enum word {
    Color_BLACK = RGB(0,0,0),
    Color_WHITE = RGB(255,255,255),
    Color_RED = RGB(255,0,0),
    Color_GREEN = RGB(0,255,0),
    Color_BLUE = RGB(0,0,255)
} Color;

void screen_init();

void wipe_screen();

void draw_pixel(word x,word y,word color);

void draw_rect(word x,word y, word width,word height, word color);

void write_string(word x,word y,char* string, word color, word background);

void draw_glyph(word x,word y,char character,word color,word background);

void decode_line(word* pointer,char** char_ptr,word color,word background);

void map_screen_buffer();

