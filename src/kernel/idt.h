#include <typedefs.h>

#ifndef _IDT
#define _IDT

typedef struct {
	u16    isr_low;      // The lower 16 bits of the ISR's address
	u16    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	u8	   ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	u8     flags;        // Type and attributes; see the IDT page
	u16    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	u32    isr_high;     // The higher 32 bits of the ISR's address
	u32    reserved;     // Set to zero
} __attribute__((packed)) idt_entry_t;

typedef struct {
	u16	limit;
	u64	base;
} __attribute__((packed)) idtr_t;

idt_entry_t idt[];
idtr_t idtr;

void init_idt();

#endif
