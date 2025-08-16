#include "io.h"
#include "ps2.h"
#include "pic.c" 
extern void pic_eoi(int irq);

#define PS2_DATA 0x60
#define PS2_STAT 0x64
#define PS2_CMD  0x64

static void ps2_wait_in(void){
   
    for (int i=0;i<100000;i++) { if (!(inb(PS2_STAT) & 0x02)) return; }
}
static void ps2_wait_out(void){
   
    for (int i=0;i<100000;i++) { if (inb(PS2_STAT) & 0x01) return; }
}

static void ps2_write_cmd(uint8_t cmd){ ps2_wait_in(); outb(PS2_CMD, cmd); }
static void ps2_write_data(uint8_t v){ ps2_wait_in(); outb(PS2_DATA, v); }
static uint8_t ps2_read_data(void){ ps2_wait_out(); return inb(PS2_DATA); }

static void mouse_write(uint8_t v){
    ps2_write_cmd(0xD4);
    ps2_write_data(v);
}
static uint8_t mouse_read(void){ return ps2_read_data(); }

volatile int mouse_x = 100, mouse_y = 100;
volatile uint8_t mouse_btn = 0;

static uint8_t mpack[4];
static int mindex = 0;

void ps2_init(void){
    
    ps2_write_cmd(0xAD); 
    ps2_write_cmd(0xA7); 

    
    (void)inb(PS2_DATA);

   
    ps2_write_cmd(0x20);
    uint8_t cfg = ps2_read_data();

    
    cfg |=  (1<<0);  
    cfg |=  (1<<1);  
    cfg &= ~(1<<6);  
    ps2_write_cmd(0x60);
    ps2_write_data(cfg);

  
    ps2_write_cmd(0xAE);
    ps2_write_cmd(0xA8);

    
    mouse_write(0xF6); (void)mouse_read(); 
    mouse_write(0xF4); (void)mouse_read(); 

    
    ps2_write_data(0xF0); (void)ps2_read_data();
    ps2_write_data(0x02); (void)ps2_read_data();
}

void kbd_on_irq(void){
    uint8_t sc = inb(PS2_DATA);
    (void)sc; 
    pic_eoi(1);
}

void mouse_on_irq(void){
    uint8_t b = inb(PS2_DATA);
    
    mpack[mindex++] = b;
    if (mindex == 3){
        mindex = 0;
        int dx = (int8_t)((mpack[0]&0x10)? (mpack[1]-256) : mpack[1]);
        int dy = (int8_t)((mpack[0]&0x20)? (mpack[2]-256) : mpack[2]);
        mouse_btn = mpack[0] & 0x07;
        mouse_x += dx;
        mouse_y -= dy;
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x > 1023) mouse_x = 1023;
        if (mouse_y > 767)  mouse_y = 767;
    }
    pic_eoi(12);
}
