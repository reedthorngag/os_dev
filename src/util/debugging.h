#include <typedefs.h>
#include <convertions.h>

#ifndef _DEBUGGING
#define _DEBUGGING

#define debug(x) _Generic((x),\
                            u8: debug_u8, \
                            u16: debug_u16, \
                            u32: debug_u32,  \
                            u64: debug_u64,   \
                            char*: debug_str   \
                            )(x);

#define debug_(x,j) debug(x);debug(j);

void debug_bool(bool out);

void debug_u8(u8 b);

void debug_u16(u16 out);

void debug_u32(u32 out);

void debug_u64(u64 out);

void debug_str(char str[]);

#endif
