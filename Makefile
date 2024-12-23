OUT_DIR := build
KERNEL_SOURCES := src/kinit.S src/kmain.c src/utils.c src/ports.c src/vga_screen.c \
									src/idt.c src/irq_handler.c src/isr.S
BOOT_FILE := src/boot.S

CC := i686-elf-gcc
CFLAGS := -g -ffreestanding -nostdlib -Isrc -c -O2
AS := nasm
ASFLAGS := -g -f elf -Isrc
ASFLAGS_BIN := -f bin -Isrc
LD := i686-elf-gcc
LDFLAGS := -T link.ld -ffreestanding -nostdlib -lgcc -O2
OBJCOPY := i686-elf-objcopy

$(shell mkdir -p $(OUT_DIR))

OBJS := $(patsubst src/%.c,$(OUT_DIR)/%.o,$(filter %.c,$(KERNEL_SOURCES))) \
				$(patsubst src/%.S,$(OUT_DIR)/%.o,$(filter %.S,$(KERNEL_SOURCES)))

all: $(OUT_DIR)/ssos.img

$(OUT_DIR)/ssos.img: $(OUT_DIR)/boot.bin $(OUT_DIR)/kernel.bin
	cat $^ > $@

$(OUT_DIR)/kernel.bin: $(OUT_DIR)/kernel.elf
	$(OBJCOPY) -O binary $^ $@

$(OUT_DIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OUT_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT_DIR)/%.o: src/%.S
	$(AS) $(ASFLAGS) -o $@ $<

$(OUT_DIR)/boot.bin: $(BOOT_FILE)
	$(AS) $(ASFLAGS_BIN) -o $@ $<

clean:
	rm -rf $(OUT_DIR)
