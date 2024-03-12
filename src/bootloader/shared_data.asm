
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

global pml_space_start
global pml_space_end
pml_space_start: dq 0
pml_space_end: dq 0

global physical_kernel_start
physical_kernel_start: dq 0

global mem_map_size
mem_map_size: dw 0
global mem_map_buffer
mem_map_buffer:

times 0x400-($-$$) db 0
global mem_map_buffer_end
mem_map_buffer_end:
