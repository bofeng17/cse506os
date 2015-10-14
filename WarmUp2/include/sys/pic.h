#ifndef _PIC_H
#define _PIC_H

#include <sys/defs.h>
#include <sys/io.h>

//----------------------pic--------------------------------------------
#define PIC1			0x20		/* IO base address for master PIC */
#define PIC2			0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA		(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2+1)
#define PIC_EOI			0x20		/* End-of-interrupt command code */

static inline void pic_sendEOI(uint8_t irq) {
    if(irq >= 40)
        outb(PIC2_COMMAND,PIC_EOI);
    outb(PIC1_COMMAND,PIC_EOI);
}

static inline void pic_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
    
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);
}

static inline void pic_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
    
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}

static inline void pic_remap() {
    __asm__ __volatile__("cli");
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    for (int i = 0; i<16; i++) {
        pic_clear_mask(i);
    }
    __asm__ __volatile__("sti");
}


#endif
