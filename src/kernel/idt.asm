[BITS 64]

extern default_handler
extern keyboard_handler
extern irq_handler

%macro isr_stub 1
isr_stub_%+%1:
    ;mov rsi,%+%1
    call keyboard_handler
    ;iretq
%endmacro

isr_stub_0:
    call keyboard_handler
    iretq

%assign i 1
%rep    255
isr_stub_%+i:
    ;mov rsi,%+%1
    call keyboard_handler
    iretq
%assign i i+1 
%endrep

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dq isr_stub_%+i
%assign i i+1 
%endrep

