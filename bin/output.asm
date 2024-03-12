    [BITS 16]
section .boot
global start
start:
    cli
	xor ax, ax
	mov es, ax
	mov ss, ax	; intialize stack to 0x0000:0x7C00
			    ; (directly below bootloader)
	sti
	xor ax,ax
	mov ds, ax		; this should already be set, but better safe than sorry
    mov [drive_number],dl
    mov si,disk_address_packet
    call read_lba_blocks
    mov bx,VBE_controller_info
    call print_hex
    mov bx,VBE_mode_info
    call print_hex
    call get_mem_map
    mov bx, [second_stage_start]
    call print_hex
    call pause
    call setup_VESA_VBE
    jmp drop_into_long_mode

hex_characters: db '0123456789abcdef'
; number to print in bx
; preserves all registers
print_hex:
	push ax
	push bx
	push dx
	mov ax,bx
	mov bx,0x1000
	xor dx,dx	; this is necessery for some reason (div instruction dies without it)
.hex_print_loop:
	div bx		; divide ax by bx, quotent in ax, remainder in dx
	push bx
	mov bx,ax
	mov al,[hex_characters+bx]
	mov bh,0
	mov ah,0x0e
	int 0x10
	pop ax
	push dx
	xor dx,dx
	mov bx,0x10
	div bx
	mov bx,ax
	pop ax
	cmp bx,0x00
	jne .hex_print_loop
.end:
	mov bh,0
	mov ax,0x0e20
	int 0x10		; add a space at the end for nice output
	pop dx
	pop bx
	pop ax
	ret
hang:
    cli
    hlt
print_str:
    mov ah,0x0e
.loop:
    lodsb
    cmp al,0
    je .end
    int 0x10
    jmp .loop
.end:
    ret
; pauses until a key is pressed
; preserves all registers
global pause
pause:
    push ax
.wait_for_key_loop:
    hlt
    mov ah,0x01
    int 0x16
    jz .wait_for_key_loop
    pop ax
    ret


read_lba_blocks:
	mov dl,[drive_number]
	mov ah,0x42
	int 0x13
	jc .failed
	ret
.failed:
	mov bx,ax
	call print_hex
	call hang
; lba disk address packet
disk_address_packet:
	db 0x10
	db 0x00
.number_of_blocks:
	dw 0x80
.transfer_buffer_offset:
	dw 0x0000
.transfer_buffer_segment:
	dw 0x07e0
.LBA_address:
	dq 1
	dq 0

    times 510-($-$$) db 0
    dw 0xaa55
bootloader_end:

extern kernel_start
drop_into_long_mode:
    ; activate A20
    mov ax,0x2403
    int 0x15
    mov eax,0x80000000
    cpuid
    cmp eax,0x80000001
    jb .no_long_mode    ; extended functions not available
    mov eax,0x80000001
    cpuid
    test edx,1<<29
    jz .no_long_mode    ; long mode not available
    jmp .long_mode
.no_long_mode:
    cli
    hlt
.long_mode:
    ; setup page tables stuff
    mov edi,0x1000
    mov cr3,edi
    xor eax,eax
    mov ecx,0x1000
    rep stosd
    mov edi,cr3
    mov dword [edi],0x00002003
    add edi,0x1000
    mov dword [edi],0x00003003
    add edi,0x1000
    mov dword [edi],0x00004003
    add edi,0x1000
    mov eax,0x00000003
    mov ecx,0x200
.add_entry:
    mov dword [edi],eax
    add eax,0x1000
    add edi,8
    loop .add_entry
    mov eax,cr4
    or eax,1<<5
    mov cr4,eax
    mov ecx,0xc0000080
    rdmsr
    or eax,1<<8
    wrmsr
    cli
    lgdt [GDT.desc]
    mov ax,GDT.data
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov eax,cr0
    or eax,(1<<31) | (1<<0)
    mov cr0,eax
    jmp GDT.code:long_mode
[BITS 64]
long_mode:
    mov rsi,second_stage_start
    jmp rsi
[BITS 16]
; Access bits
PRESENT  equ 1 << 7
NOT_SYS  equ 1 << 4
EXEC     equ 1 << 3
DC       equ 1 << 2
RW       equ 1 << 1
ACCESSED equ 1 << 0
 
; Flags bits
GRAN_4K    equ 1 << 7
SZ_32      equ 1 << 6
LONG_MODE  equ 1 << 5
GDT:
    .null: equ $ - GDT
        dq 0
    .code: equ $ - GDT
        dd 0xFFFF                                   ; Limit
        db 0                                        ; Base
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .data: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .TSS: equ $ - GDT
        dd 0x00000068
        dd 0x00CF8900
    .desc:
        dw $ - GDT - 1
        dq GDT


get_mem_map:
	db 0xff, 0xfe
    mov di, mem_map_buffer
	xor ebx, ebx		; ebx must be 0 to start
	xor bp, bp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov dword [es:di + 20], 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .failed	; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne short .failed
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov dword [es:di + 20], 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	call print_hex
	jc short .e820f		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent		; skip any 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .notext
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipent		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24
    cmp di,mem_map_buffer_end
    jge .ran_out_of_space
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	mov word [mem_map_size], bp	; store the entry count
	ret
.failed:
    mov si, .gmm_err_str
    call print_str
    call hang
.ran_out_of_space:
    mov si, .out_of_space_err_str
    call print_str
	mov bx,bp
	call print_hex
    call hang
.gmm_err_str: db 'function unsupported! ',0
.out_of_space_err_str: db 'ran out of space to read mem map into!',0


read_acpi_tables:
    


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
    .attributes:        dw 0xfffe
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

second_stage_start:
