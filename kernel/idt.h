#pragma once
#include <stdint.h>

typedef struct __attribute__((packed)){
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} idt_entry_t;

typedef struct __attribute__((packed)){
    uint16_t limit;
    uint64_t base;
} idt_ptr_t;

void idt_init(void);
void idt_set_gate(int vec, void* isr, uint8_t flags);
extern void lidt(void* idt_ptr);


void irq0_handler(void);
void irq1_handler(void);
void irq12_handler(void);
