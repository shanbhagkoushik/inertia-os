#include <stdint.h>
#include "io.h"
#include "pic.c" 
extern void pic_eoi(int irq);

static volatile uint64_t ticks = 0;

void pit_init(uint32_t hz){
    uint32_t divisor = 1193182 / hz;
    outb(0x43, 0x36);               
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void pit_on_irq(void){
    ticks++;
    pic_eoi(0);
}

uint64_t pit_ticks(void){ return ticks; }
