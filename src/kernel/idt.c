#include <idt.h>
#include <debugging.h>

extern void* isr_stub_table[];
extern void* isr_stub_0;

__attribute__((aligned(0x10))) idt_entry_t idt[256];

void idt_install_irq(u8 index, u64 irq_addr, u16 flags) {
	idt[index].isr_low = (u16)(irq_addr & 0xFFFF);
	idt[index].kernel_cs = 0x08;
	idt[index].ist = 0;
	idt[index].flags = flags;
	idt[index].isr_mid = (u16)((irq_addr >> 16) & 0xFFFF);
    idt[index].isr_high = (u32)((irq_addr >> 32) & 0xFFFFFFFF);
}

idtr_t idtr;

volatile void default_handler(u64 irq) {
    (void)irq;
    //debug(irq);
}

volatile void keyboard_handler() {
    //u8 key;
    //inb(0x60,key);
    //debug_u8(key);
    debug_str("hello\n");
}

void init_idt() {
    debug("hello");
    idtr.base = 0;//(u64)idt;
    idtr.limit = sizeof(idt_entry_t)*256-1;

    for (u8 i = 0; i < 256; i++) {
        idt_install_irq(i,(u64)0,0x8E);
    }

    __asm__ volatile ("cli");
    __asm__ volatile ("lidt %0"::"m"(idtr));
    __asm__ volatile ("sti");
}
