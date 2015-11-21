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

void cpu_exception_handler ();
void page_fault_handler ();

extern void exception0();
extern void exception1();
extern void exception2();
extern void exception3();
extern void exception4();
extern void exception5();
extern void exception6();
extern void exception7();
extern void exception8();
extern void exception9();
extern void exception10();
extern void exception11();
extern void exception12();
extern void exception13();
extern void exception14();
extern void exception15();
extern void exception16();
extern void exception17();
extern void exception18();
extern void exception19();
extern void exception20();
extern void exception21();
extern void exception22();
extern void exception23();
extern void exception24();
extern void exception25();
extern void exception26();
extern void exception27();
extern void exception28();
extern void exception29();
extern void exception30();
extern void exception31();

extern void isr32();//defined in isr.s
extern void isr33();//defined in isr.s

#endif
