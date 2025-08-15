
.set MBOOT2_MAGIC, 0xE85250D6
.set ARCH_X86,     0

    .section .multiboot2_header,"a"
    .align 8
start:
    
    .long MBOOT2_MAGIC
    .long ARCH_X86
    .long (end - start)             
    .long -(MBOOT2_MAGIC + ARCH_X86 + (end - start))  

    .align 8
    .short 5         
    .short 0          
    .long 20          
    .long 1024        
    .long 768        
    .long 32         

    
    .align 8
    .short 0
    .short 0
    .long 8
end:
