[BITS 64]

extern default_handler
extern keyboard_handler
extern irq_handler

%macro pusha 0
    push rax
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
%endmacro

%macro popa 0
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rax
%endmacro

%macro isr_stub 2
isr_stub_%+%1:
    ;pusha
    ;push rsi
    ;mov rsi,%+%1
    cli
    hlt
    ;call default_handler
    ;pop rsi
    ;popa
%if %2 == 0
    add rsp,8 ; pop error code
%endif
    iretq
%endmacro

%macro isr_stub_range 2
%assign i %1
%rep    %2
isr_stub_%+i:
    ;pusha
    ;in al,0x60
    ; mov ax,0xe9
    ; out 0x61,ax
    mov al,0x20
    out 0x20,al
    ;popa
    iretq
%assign i i+1 
%endrep
%endmacro

isr_stub 0,0
isr_stub 1,0
isr_stub 2,0
isr_stub 3,0
isr_stub 4,0
isr_stub 5,0
isr_stub 6,0
isr_stub 7,0

isr_stub 8,1
isr_stub 9,1
isr_stub 10,1
isr_stub 11,1
isr_stub 12,1
isr_stub 13,1
isr_stub 14,1
isr_stub 15,1

isr_stub 16,0

isr_stub 17,1

isr_stub 18,0
isr_stub 19,0
isr_stub 20,0

isr_stub_range 21,9

isr_stub 30,1

isr_stub_31:
    pusha
    mov al,0x61
    out 0xe9,al
    mov al,0x20
    out 0x20,al
    popa
    iretq

isr_stub_32:
    iretq

isr_stub_range 33,224


global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    256
    dq isr_stub_%+ i
%assign i i+1 
%endrep

