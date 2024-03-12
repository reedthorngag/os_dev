
#include <typedefs.h>
#include <convertions.h>

#ifndef _DEBUGGING
#define _DEBUGGING

#define debug(x) _Generic((x),\
                            u8: debug_binary, \
                            u16: debug_short, \
                            u32: debug_int,     \
                            u64: debug_long,   \
                            char*: debug_str   \
                            )(x);

#define debug_(x,j) debug(x);debug(j);

void debug_bool(bool out);

void debug_binary(u8 b);

void debug_short(u16 out);

void debug_int(u32 out);

void debug_long(u64 out);

void debug_str(char str[]);

#endif
