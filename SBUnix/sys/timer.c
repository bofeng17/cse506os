#include <sys/sbunix.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include <sys/printf.h>
#include <sys/process.h>

volatile uint32_t system_boot_mS = 0; //Number of whole mS since timer initialized
uint32_t IRQ0_fractions = 999.847746; //Fractions of 1 mS between IRQs
//uint32_t IRQ0_period=1; //mS between IRQs, accurate: 999.847746 ms
//uint16_t PIT_reload_value = 1193; //Current PIT reload value 1193. accurate: 1000.152277 HZ

//the next two lines are modified during building preemptive scheduling
uint32_t IRQ0_period = 10; //10 mS between IRQs
uint16_t PIT_reload_value = 11930; //100 HZ

uint32_t interrupt_count = 0;

//Every time the mode/command register is written to, all internal logic in the selected PIT channel is reset, and the output immediately goes to its initial state
void
timer_init ()
{
    
    // Send the command byte.
    outb (0x43, 0x36); //channel 0, lobyte/hibyte, mode 2:110b
    
    // Divisor has to be sent byte-wise
    uint8_t lobyte = (uint8_t) (PIT_reload_value & 0xFF);
    uint8_t hibyte = (uint8_t) ((PIT_reload_value >> 8) & 0xFF);
    
    // Send the frequency divisor.
    outb (0x40, lobyte);
    outb (0x40, hibyte);
    //      pic_set_mask (0);
    //      pic_set_mask (1);
}

//extern task_struct* idle;

//extern task_struct* testa;
//extern task_struct* testb;
//extern task_struct* testc;
//extern void context_switch(task_struct *,task_struct *);
uint64_t boot_count = 0;
uint64_t init_count = 0;
uint64_t count = 0;

void
isr_timer ()
{
    size_t _console_row = console_row;
    size_t _console_column = console_column;
    console_row = 24;
    console_column = 67;
    if (interrupt_count % 6000 != 0)
    { //To be precise, because the Period is 0.999847746ms instead of 1ms
        system_boot_mS += IRQ0_period;
    }
    pic_sendEOI (33);
    interrupt_count++;
    printf ("%2d:%2d:%2d.%3d", system_boot_mS / 1000 / 60 / 60,
            system_boot_mS / 1000 / 60 % 60, system_boot_mS / 1000 % 60,
            system_boot_mS % 1000);
    console_row = _console_row;
    console_column = _console_column;
    
    boot_count++;
    
    // for cooperative scheduling:
//    schedule();
}
