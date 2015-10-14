#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>

// for IDT

struct idt_entry_struct{
   uint16_t base_lo;           // The lower 16 bits of the address to jump to when this interrupt fires.
   uint16_t sel;               // Kernel segment selector.
   uint8_t  always0;           // This must always be zero.
   uint8_t  flags;             // More flags. See documentation.
   uint16_t base_hi;           // The upper 16 bits of the address to jump to.
   uint32_t base_64;           // The extended 32 bits of the address to jump to.
   uint32_t reserved;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idtr_t {
	uint16_t size;
	uint64_t addr;
}__attribute__((packed));

extern idt_entry_t idt[];


extern void idt_init();
extern void reload_idt();
extern void idt_set_gate(uint8_t, uint64_t, uint16_t, uint8_t);

// These extern directives let us access the addresses of our ASM ISR handlers.
extern void isr0();
extern void isr1();

extern void isr32();//defined in isr.s
extern void isr33();//defined in isr.s

#endif
