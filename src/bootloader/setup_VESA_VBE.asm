
global setup_VESA_VBE
setup_VESA_VBE:

    xor ax,ax
    mov es,ax

    mov ax,0x4f00
    mov di,VBE_controller_info
    int 0x10

    cmp ax,0x004f
    jne .VESA_VBE_failed

    mov si,[VBE_controller_info.video_modes_ptr]

.find_end_loop:
    mov cx,[si]
    cmp cx,0xffff
    je .loop
    add si,2
    jmp .find_end_loop

.loop:
    sub si,2

    cmp si,[VBE_controller_info.video_modes_ptr]
    je .no_supported_modes

    mov cx,[si]

    mov di,VBE_mode_info

    mov ax,0x4f01
    int 0x10

    cmp byte [VBE_mode_info.bits_per_pixel],0x0f
    jne .loop

    mov al,[VBE_mode_info.attributes]
    and al,0x10
    jz .loop

    mov bx,cx
    mov ax,0x4f02
    int 0x10

    cmp ah,0
    jne .loop

.end:
    mov ax,[VBE_mode_info.x_res]
    mov word [screen_res_x],ax
    mov ax,[VBE_mode_info.y_res]
    mov word [screen_res_y],ax

    mov eax,[VBE_mode_info.mem_base_ptr]
    mov dword [screen_buffer_ptr_real],eax

    xor eax,eax
    mov ax,[VBE_mode_info.win_mem]
    shl eax,10
    mov dword [screen_buffer_size],eax

    mov ax,[VBE_mode_info.bytes_per_scanline]
    mov word [bytes_per_line],ax

    ret


.no_supported_modes:
    mov si,VBE_errors.no_supported_modes
    call print_str
    call hang

.VESA_VBE_failed:
    mov si,VBE_errors.controller_info_failed
    call print_str
    mov bx,ax
    call print_hex
    call hang

VBE_errors:
    .controller_info_failed db 'ERR: failed to get VESA VBA controller info! error data: ',0
    .no_supported_modes db 'ERR: no supported video modes!',0


VBE_controller_info:
    .signature      db 'VESA'
    .version        dw 0x0200
    .OEM_str_ptr    dd 0
    .capabilities   dd 0
    .video_modes_ptr dd 0
    .total_mem      dw 0 ; num of 64Kib blocks
    dw 0xffff

    .extra_data: times 0x200-($-VBE_controller_info) db 0


current_VBE_mode dw 0

VBE_mode_info:
    .attributes:        dw 0
    .win_A_attributes   db 0
    .win_B_attributes   db 0
    .granularity        dw 0    ; KB
    .win_mem            dw 0    ; KB
    .start_seg_win_A    dw 0    ; 0 if unsupported
    .start_seg_win_B    dw 0    ; 0 if unsupported
    .win_func_ptr       dd 0    ; not quite sure what this is, something to do with int 10h/ax 4f05h?
    .bytes_per_scanline dw 0
    
    .x_res              dw 0
    .y_res              dw 0
    .char_cell_width    db 0
    .char_cell_height   db 0
    .num_planes         db 0    ; number of memory planes
    .bits_per_pixel     db 0
    .num_banks          db 0    ; number of banks
    .memory_model_type  db 0    ; http://www.ctyme.com/intr/rb-0274.htm#Table82
    .bank_size          db 0    ; size of bank in KB
    .num_image_pages    db 0    ; zero based number of image pages that will fit in video ram
                        db 0    ; reserved

    .red_mask_size      db 0
    .red_field_pos      db 0
    .green_mask_size    db 0
    .green_field_pos    db 0
    .blue_mask_size     db 0
    .blue_field_pos     db 0
    .reserved_mask_size db 0
    .reserved_mask_pos  db 0
    .direct_color_info  db 0    ; direct color mode info

    .mem_base_ptr       dd 0    ; address of video buffer
    .off_scrn_mem_ptr   dd 0    ; address of off screen memory
    .off_scrn_mem_size  dw 0    ; size of off screen memory in KB
