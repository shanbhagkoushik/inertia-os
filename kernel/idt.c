#include "idt.h"
#include <stddef.h>
#include <stdint.h>

#define IDT_TYPE_INTGATE 0x8E /* present | DPL=0 | type=0xE */
static idt_entry_t IDT[256];
static idt_ptr_t   IDTP;

extern void isr_stub_irq0(void);
extern void isr_stub_irq1(void);
extern void isr_stub_irq12(void);
extern void isr_stub_default(void);

void idt_set_gate(int vec, void* isr, uint8_t flags){
    uint64_t addr = (uint64_t)isr;
    IDT[vec].offset_low  = addr & 0xFFFF;
    IDT[vec].selector    = 0x08;     
    IDT[vec].ist         = 0;
    IDT[vec].type_attr   = flags;
    IDT[vec].offset_mid  = (addr >> 16) & 0xFFFF;
    IDT[vec].offset_high = (addr >> 32);
    IDT[vec].zero        = 0;
}

void idt_init(void){
    for (int i=0;i<256;i++) idt_set_gate(i, isr_stub_default, IDT_TYPE_INTGATE);
    idt_set_gate(32, isr_stub_irq0,  IDT_TYPE_INTGATE); 
    idt_set_gate(33, isr_stub_irq1,  IDT_TYPE_INTGATE); 
    idt_set_gate(44, isr_stub_irq12, IDT_TYPE_INTGATE); 

    IDTP.limit = sizeof(IDT)-1;
    IDTP.base  = (uint64_t)&IDT[0];

    __asm__ __volatile__("lidt %0"::"m"(IDTP));
    __asm__ __volatile__("sti");
}
