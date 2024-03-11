
section .kernel_data

global screen_res_x
global screen_res_y
global screen_buffer_ptr_real
global virtual_scrn_buf_ptr
global screen_buffer_size
global bytes_per_line
global bytes_per_pixel

screen_res_x dw 0
screen_res_y dw 0
screen_buffer_ptr_real dd 0
virtual_scrn_buf_ptr dd 0
screen_buffer_size dd 0
bytes_per_line dw 0
bytes_per_pixel db 2

global drive_number
drive_number: db 0
