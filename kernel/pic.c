#include <stdint.h>
#include "io.h"

#define PIC1    0x20
#define PIC2    0xA0
#define PIC1_CMD PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_CMD PIC2
#define PIC2_DATA (PIC2+1)
#define PIC_EOI  0x20

static void pic_remap(int off1, int off2){
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_CMD, 0x11); io_wait();
    outb(PIC2_CMD, 0x11); io_wait();

    outb(PIC1_DATA, off1); io_wait();
    outb(PIC2_DATA, off2); io_wait();

    outb(PIC1_DATA, 0x04); io_wait();
    outb(PIC2_DATA, 0x02); io_wait();

    outb(PIC1_DATA, 0x01); io_wait();
    outb(PIC2_DATA, 0x01); io_wait();

    outb(PIC1_DATA, a1); 
    outb(PIC2_DATA, a2);
}

void pic_init(void){
    pic_remap(0x20, 0x28);
   
    outb(PIC1_DATA, 0x00 & ~(1<<0) & ~(1<<1)); 
    outb(PIC1_DATA, 0xFC); 
    outb(PIC2_DATA, 0xEF); 
}

void pic_eoi(int irq){
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}
