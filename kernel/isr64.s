.intel_syntax noprefix
.section .text
.code64
.global isr_stub_irq0
.global isr_stub_irq1
.global isr_stub_irq12
.global isr_stub_default

.macro ISR_STUB label, handler
\label:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    call \handler
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq
.endm

.extern irq0_handler
.extern irq1_handler
.extern irq12_handler

ISR_STUB isr_stub_irq0,  irq0_handler
ISR_STUB isr_stub_irq1,  irq1_handler
ISR_STUB isr_stub_irq12, irq12_handler

isr_stub_default:
    iretq
