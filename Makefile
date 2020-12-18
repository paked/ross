SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -I. -ffreestanding -nostdlib -nostartfiles

all: clean kernel.bin

bootloaded.o: bootloaded.S
	aarch64-none-elf-gcc $(CFLAGS) -c bootloaded.S -o bootloaded.o

%.o: %.c
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@

kernel.bin: bootloaded.o $(OBJS)
	aarch64-none-elf-ld -nostdlib -nostartfiles bootloaded.o $(OBJS) -T link.ld -o kernel.elf
	aarch64-none-elf-objcopy -O binary kernel.elf kernel.bin

clean:
	rm kernel.elf kernel.bin *.o >/dev/null 2>/dev/null || true
