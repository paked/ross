#include <stdbool.h>
#include <stdint.h>

#define MMIO_BASE       0x3F000000

/* Auxilary mini UART registers */
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))

#define GPFSEL0         ((volatile unsigned int*)(MMIO_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(MMIO_BASE+0x00200008))
#define GPFSEL3         ((volatile unsigned int*)(MMIO_BASE+0x0020000C))
#define GPFSEL4         ((volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPFSEL5         ((volatile unsigned int*)(MMIO_BASE+0x00200014))
#define GPSET0          ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPSET1          ((volatile unsigned int*)(MMIO_BASE+0x00200020))
#define GPCLR0          ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPLEV0          ((volatile unsigned int*)(MMIO_BASE+0x00200034))
#define GPLEV1          ((volatile unsigned int*)(MMIO_BASE+0x00200038))
#define GPEDS0          ((volatile unsigned int*)(MMIO_BASE+0x00200040))
#define GPEDS1          ((volatile unsigned int*)(MMIO_BASE+0x00200044))
#define GPHEN0          ((volatile unsigned int*)(MMIO_BASE+0x00200064))
#define GPHEN1          ((volatile unsigned int*)(MMIO_BASE+0x00200068))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(MMIO_BASE+0x0020009C))

void uart_init() {
	// 
	// GPIO stuff
	//

	// use alt 5 (010) for pin 14 and 15 

	*GPFSEL1 &= ~((0b111 << 12)|(0b111 << 15));
	*GPFSEL1 |= (0b010 << 12)|(0b010 << 15);

	*GPPUD = 0;
	int i=150; while(i--) { asm volatile("nop"); }

	*GPPUDCLK0 = (1<<14)|(1<<15);
	i=150; while(i--) { asm volatile("nop"); }

	*GPPUDCLK0 = 0;

	//
	// mini USART stuff
	//

	// enable module (EM3)
	*AUX_ENABLE &= 1 << 0;
	*AUX_ENABLE |= 1 << 0;

	// disable rx and tx (CNTL)
	*AUX_MU_CNTL &= ~(0b11);

	// disable interrupts (IER)
	*AUX_MU_IIR &= ~((1<<1)|(1<<0));

	// switch to 8 bit mode (LCR)
	*AUX_MU_LCR &= ~((1<<1)|(1<<0));
	*AUX_MU_LCR |= ((1<<1)|(1<<0));

	// clear FIFO queues for rx and tx (IIR)
	*AUX_MU_IIR |= (0b11 << 1);

	// set baud rate
	*AUX_MU_BAUD = 270;

	// enable rx and tx (CNTL)
	*AUX_MU_CNTL |= (0b11);
}

void uart_putc(char c) {
	do {
		asm volatile("nop");
	} while (!(*AUX_MU_LSR&0x20));

	*AUX_MU_IO = c;
}

char uart_getc() {
	do {
		asm volatile("nop");
	} while (!(*AUX_MU_LSR&0b1));

	return *AUX_MU_IO;
}

void bootloaded(void) {
	uart_init();

	while (true) {
		uart_putc('F');
	}
}
