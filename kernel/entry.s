
.intel_syntax noprefix
.section .text
.global _start
.extern kmain           
.global mboot_ptr64

_start:
    cli

    
    mov eax, 0          
    mov [mboot_ptr64], ebx
    mov [mboot_ptr64+4], eax

    
    mov eax, cr4
    or  eax, 1 << 5
    mov cr4, eax

    
    lea eax, [pml4]
    mov cr3, eax

    
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    
    mov eax, cr0
    or  eax, 1 << 31
    or  eax, 1 << 0
    mov cr0, eax

    
    ljmp 0x08, long_mode_entry

.code64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    
    mov rdi, qword ptr [mboot_ptr64]
    call kmain

.hang:
    hlt
    jmp .hang


.align 4096
pml4:
    .quad pdpt | 0x03
    .zero 510*8
    .quad 0

.align 4096
pdpt:
    
    .quad (0x000000000) | (1<<7) | 0x03
    .zero 511*8


.bss
.align 8
mboot_ptr64:
    .quad 0
