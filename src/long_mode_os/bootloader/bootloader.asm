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

	mov ax, 0x07c0
	mov ds, ax		; this should already be set, but better safe than sorry


    mov [drive_number],dl

    ;call _main
    call setup_VESA_VBE


#include "utils.asm"

    times 510-($-$$) db 0
    dw 0xaa55

#include "setup_VESA_VBE.asm"
#include "get_mem_map.asm"
#include "read_acpi_tables.asm"


global drive_number
drive_number: db 0

extern _main

bootloader_end:


