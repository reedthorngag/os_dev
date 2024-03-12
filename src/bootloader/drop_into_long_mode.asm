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

