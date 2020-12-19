#include "uart.h"

#include "bcm2385.h"

void uart_init() {
	// 
	// GPIO stuff
	//

	// use alt 5 (010) for pin 14 and 15 

	GPIO->FSEL[1] &= ~((0b111 << 12)|(0b111 << 15));
	GPIO->FSEL[1] |= (0b010 << 12)|(0b010 << 15);

	GPIO->UD = 0;
	int i=150; while(i--) { asm volatile("nop"); }

	GPIO->UDCLK[0] = (1<<14)|(1<<15);
	i=150; while(i--) { asm volatile("nop"); }

	GPIO->UDCLK[0] = 0;

	//
	// mini USART stuff
	//

	// enable module (EM3)
	AUX->ENABLE &= 1 << 0;
	AUX->ENABLE |= 1 << 0;

	// disable rx and tx (CNTL)
	UART1->CNTL &= ~(0b11);

	// disable interrupts (IER)
	UART1->IER &= ~((1<<1)|(1<<0));

	// switch to 8 bit mode (LCR)
	UART1->LCR &= ~((1<<1)|(1<<0));
	UART1->LCR |= ((1<<1)|(1<<0));

	// clear FIFO queues for rx and tx (IIR)
	UART1->IIR |= (0b11 << 1);

	// set baud rate
	UART1->BAUD = 270;

	// enable rx and tx (CNTL)
	UART1->CNTL |= (0b11);
}

void uart_putc(char c) {
	do {
		asm volatile("nop");
	} while (!(UART1->LSR&(1 << 5)));

	UART1->IO = c;
}

char uart_getc() {
	do {
		asm volatile("nop");
	} while (!(UART1->LSR&(1 << 0)));

	return UART1->IO;
}
