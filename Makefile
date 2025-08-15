TARGET   := x86_64
CC       := clang
LD       := ld.lld

CFLAGS := -ffreestanding -fno-builtin -fno-stack-protector -fno-pic \
          -mno-sse -mno-sse2 -mno-red-zone -m64 -O2 -Wall -Wextra
LDFLAGS := -nostdlib -z max-page-size=0x1000

BUILD   := build
KERNEL  := $(BUILD)/kernel.elf

SRC_S  := boot/multiboot2_header.s kernel/entry.s
SRC_C  := kernel/kmain.c

OBJ    := $(patsubst %.s,$(BUILD)/%.o,$(SRC_S)) $(patsubst %.c,$(BUILD)/%.o,$(SRC_C))

all: iso

$(BUILD)/boot/%.o: boot/%.s
	@mkdir -p $(dir $@)
	clang -c $< -o $@ -m32

$(BUILD)/kernel/%.o: kernel/%.s
	@mkdir -p $(dir $@)
	clang -c $< -o $@ -m32

$(BUILD)/kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -T linker.ld -o $(KERNEL) $(OBJ)

iso: $(KERNEL) iso/boot/grub/grub.cfg
	@mkdir -p iso/boot
	cp $(KERNEL) iso/boot/kernel.elf
	grub-mkrescue -o kobayashi-os.iso iso

run: iso
	qemu-system-x86_64 -cdrom kobayashi-os.iso

clean:
	rm -rf $(BUILD) kobayashi-os.iso iso/boot/kernel.elf
