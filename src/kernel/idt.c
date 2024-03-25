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

volatile void default_handler() {
    debug("hi");
    //debug(irq);
}

volatile void keyboard_handler() {
    //u8 key;
    //inb(0x60,key);
    //debug_u8(key);
    debug_str("hello\n");
}

void remap_pic() {
    outb(0x20,0x11); // write ICW1 to PIC master
    outb(0xA0,0x11); // write ICW1 to PIC slave

    outb(0x21,0x20); // remap PIC master to 0x20
    outb(0xA1,0x28); // remap PIC slave to 0x28

    outb(0x21,0x04); // IRQ2 -> connection to slave
    outb(0xA1,0x02);

    outb(0x21,0x01); // write ICW4 to PIC master
    outb(0xA1,0x01);

    outb(0x21, 0x0); // enable all IRQs
    outb(0xA1, 0x0);
}

void init_idt() {
    idtr.base = (u64)idt;
    idtr.limit = sizeof(idt_entry_t)*256-1;

    for (u16 i = 0; i < 256; i++) {
        idt_install_irq((u8)i,(u64)isr_stub_table[i],0x8E);
    }

    __asm__ volatile ("cli");
    remap_pic();
    __asm__ volatile ("lidt %0"::"m"(idtr));
    __asm__ volatile ("sti");
}
