SRC_DIRS := src

SRCS := $(shell find $(SRC_DIRS) -name *.c -or -name *.S)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

CFLAGS := -Wall -Werror -Iinclude -ffreestanding -nostdlib -nostartfiles -O0 -mgeneral-regs-only

TARGET := kernel

DEVICE ?= /dev/ttyUSB0

all: clean $(TARGET).bin

%.o: %.c
	aarch64-none-elf-gcc $(CFLAGS) -o $@ -c $<

%.o: %.S
	aarch64-none-elf-gcc $(CFLAGS) -o $@ -c $<

$(TARGET).bin: $(OBJS)
	aarch64-none-elf-ld -nostdlib -nostartfiles $(OBJS) -T link.ld -o $(TARGET).elf
	aarch64-none-elf-objcopy -O binary $(TARGET).elf $(TARGET).bin

flash: $(TARGET.bin)
	python3 bootloader/tools/bootloader-client.py $(TARGET).bin $(DEVICE)
	# screen $(DEVICE) 115200
	tio $(DEVICE)

clean:
	rm $(TARGET).elf $(TARGET).bin $(OBJS) >/dev/null 2>/dev/null || true
