#include <sys/sbunix.h>
#include <sys/pic.h>
#include <sys/idt.h>

#define MAX_IDT 256

idt_entry_t idt[MAX_IDT] = {};

static struct idtr_t idtr = {
	sizeof(idt),
	(uint64_t)idt,
};

void idt_init(){
	for (int i = 0; i < MAX_IDT; i ++) {
		idt[i].base_lo = 0;
		idt[i].sel = 0;
		idt[i].always0 = 0;
		idt[i].flags = 0;
		idt[i].base_hi = 0;
		idt[i].base_64 = 0;
		idt[i].reserved = 0;
	}
    //Remap the irq table.
    //IRQ 0-15 -> INT 32-47
    pic_remap();
}

void _x86_64_asm_lidt(struct idtr_t* idtr);

void reload_idt() {
	idt_init();
	_x86_64_asm_lidt(&idtr);

    idt_set_gate(0x20, (uint64_t)isr32, 0x08, 0x8E);
	idt_set_gate(0x21, (uint64_t)isr33, 0x08, 0x8E);
// test idt
//  print idtr(register)
//	printf("%x\n",idt[0].base_lo);
//	printf("%x\n",idt[0].sel);
//	printf("%x\n",idt[0].always0);
//	printf("%x\n",idt[0].flags);
//	printf("%x\n",idt[0].base_hi);
//	idt_set_gate( 1, (uint32_t)isr1 , sel, flags);
}

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags){
	idt[num].base_lo = base & 0xFFFF;
   	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].base_64 = (base >> 32) & 0xFFFFFFFF;
	idt[num].sel     = sel;
   	idt[num].always0 = 0;
   // TODO
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   	idt[num].flags   = flags /* | 0x60 */;
}
