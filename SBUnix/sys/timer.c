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
uint32_t IRQ0_period = 5; // 5 mS between IRQs
uint16_t PIT_reload_value = 5965; // 11930/2 50 HZ

uint32_t boot_count = 0;
uint32_t ready_schedule = 0;// 1 for ready

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

void isr_timer (uint64_t cs) {
    size_t _console_row = console_row;
    size_t _console_column = console_column;
    console_row = 24;
    console_column = 67;
    
    // for printf boot_time on lower-right corner
    if (boot_count % 6000 != 0) {
        //To be precise, because the Period is 0.999847746ms instead of 1ms
        system_boot_mS += IRQ0_period;
    }
    pic_sendEOI (33);
    boot_count++;
    printf ("%2d:%2d:%2d.%3d", system_boot_mS / 1000 / 60 / 60,
            system_boot_mS / 1000 / 60 % 60, system_boot_mS / 1000 % 60,
            system_boot_mS % 1000);
    console_row = _console_row;
    console_column = _console_column;
    
    //printf("%d   ",cs);
    
    // after 2s of boot, do...
//    if (boot_count > 400) {
//        // for do_sleep
//        sleep_time_decrease ();
//        
//        /*
//         * for preemptive scheduling
//         * schedule every 1s
//         */
//        if (boot_count%200 == 0) {
//            // time slice runs up, ready to schedule
//            ready_schedule = 1;
//        }
//        if (ready_schedule && ((cs != 0x8) || (current -> pid == 0))) {
//            // if readu for schedule, and
//            // timer interrupt happens in user mode or in kernel mode executing idle thread
//            ready_schedule = 0;
//            schedule();
//        } // otherwise try to schedule during next timer interrupt
//    }
    
}
