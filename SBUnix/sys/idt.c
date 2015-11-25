#include <sys/sbunix.h>
#include <sys/pic.h>
#include <sys/idt.h>

#define MAX_IDT 256

idt_entry_t idt[MAX_IDT] =
{ };

static struct idtr_t idtr =
{ sizeof(idt), (uint64_t) idt, };

void
idt_init ()
{
    for (int i = 0; i < MAX_IDT; i++)
    {
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
    pic_remap ();
}

void
_x86_64_asm_lidt (struct idtr_t* idtr);


void
cpu_exception_handler (uint64_t exception_no)
{
    printf("CPU exception %x happens!\n",exception_no);
    __asm__ __volatile__("hlt");
}

void
reload_idt ()
{
    idt_init ();
    _x86_64_asm_lidt (&idtr);
    //register CPU exception handler exception0
    idt_set_gate (0x00, (uint64_t) exception0, 0x08, 0x8E);
    idt_set_gate (0x01, (uint64_t) exception1, 0x08, 0x8E);
    idt_set_gate (0x02, (uint64_t) exception2, 0x08, 0x8E);
    idt_set_gate (0x03, (uint64_t) exception3, 0x08, 0x8E);
    idt_set_gate (0x04, (uint64_t) exception4, 0x08, 0x8E);
    idt_set_gate (0x05, (uint64_t) exception5, 0x08, 0x8E);
    idt_set_gate (0x06, (uint64_t) exception6, 0x08, 0x8E);
    idt_set_gate (0x07, (uint64_t) exception7, 0x08, 0x8E);
    idt_set_gate (0x08, (uint64_t) exception8, 0x08, 0x8E);
    idt_set_gate (0x09, (uint64_t) exception9, 0x08, 0x8E);
    idt_set_gate (0x0A, (uint64_t) exception10, 0x08, 0x8E);
    idt_set_gate (0x0B, (uint64_t) exception11, 0x08, 0x8E);
    idt_set_gate (0x0C, (uint64_t) exception12, 0x08, 0x8E);
    idt_set_gate (0x0D, (uint64_t) exception13, 0x08, 0x8E);
    idt_set_gate (0x0E, (uint64_t) exception14, 0x08, 0x8E);
    idt_set_gate (0x0F, (uint64_t) exception15, 0x08, 0x8E);
    idt_set_gate (0x10, (uint64_t) exception16, 0x08, 0x8E);
    idt_set_gate (0x11, (uint64_t) exception17, 0x08, 0x8E);
    idt_set_gate (0x12, (uint64_t) exception18, 0x08, 0x8E);
    idt_set_gate (0x13, (uint64_t) exception19, 0x08, 0x8E);
    idt_set_gate (0x14, (uint64_t) exception20, 0x08, 0x8E);
    idt_set_gate (0x15, (uint64_t) exception21, 0x08, 0x8E);
    idt_set_gate (0x16, (uint64_t) exception22, 0x08, 0x8E);
    idt_set_gate (0x17, (uint64_t) exception23, 0x08, 0x8E);
    idt_set_gate (0x18, (uint64_t) exception24, 0x08, 0x8E);
    idt_set_gate (0x19, (uint64_t) exception25, 0x08, 0x8E);
    idt_set_gate (0x1A, (uint64_t) exception26, 0x08, 0x8E);
    idt_set_gate (0x1B, (uint64_t) exception27, 0x08, 0x8E);
    idt_set_gate (0x1C, (uint64_t) exception28, 0x08, 0x8E);
    idt_set_gate (0x1D, (uint64_t) exception29, 0x08, 0x8E);
    idt_set_gate (0x1E, (uint64_t) exception30, 0x08, 0x8E);
    idt_set_gate (0x1F, (uint64_t) exception31, 0x08, 0x8E);
    //register timer & kerboard interrupt handler
    idt_set_gate (0x20, (uint64_t) isr32, 0x08, 0x8E);
    idt_set_gate (0x21, (uint64_t) isr33, 0x08, 0x8E);
    
}

void
idt_set_gate (uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].base_64 = (base >> 32) & 0xFFFFFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    // TODO
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt[num].flags = flags /* | 0x60 */;
}
