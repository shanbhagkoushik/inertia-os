#pragma once
#include <stdint.h>

void ps2_init(void);
void kbd_on_irq(void);
void mouse_on_irq(void);

extern volatile int mouse_x, mouse_y;
extern volatile uint8_t mouse_btn;
