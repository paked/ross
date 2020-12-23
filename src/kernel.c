#include <stdbool.h>
#include <stdint.h>

#include "bcm2385.h"
#include "uart.h"
#include "util.h"

#define XPRINTF_IMPLEMENTATION
#include "xprintf.h"

#define printf(...) xprintf(&write, __VA_ARGS__)

uint32_t write(char* buf, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		if (buf[i] == '\n') {
			uart_putc('\r');
		}

		uart_putc(buf[i]);
	}

	return len;
}

void synchronous_exception(uint64_t esr) {
	uint32_t smol_esr = (uint32_t) esr;

	uint32_t ec = (smol_esr >> 26) & (0b111111);

	switch (ec) {
		case 0b010101: // SVC
			{
				uint32_t imm = smol_esr & 0xFF;

				printf("got svc %d\n", imm);
			} break;
		default:
			printf("unknown synchronous exception syndrome! cannot recover.\n");

			while(true);
	}
}

void irq_exception() {
	if (INTERRUPTS->PENDING[0] & (1 << 1)) {
		printf("timer interrupt!\n");
		SYSTIMER->C1 = SYSTIMER->CLO + 1000000; // delay for some point in the future
		SYSTIMER->CS |= (1 << 1);

	}

	printf("got IRQ exception!\n");
}

void die() {
	printf("unhandled exception called! cannot recover.\n");

	while (true);
}

void bootloaded(void) {
	uart_init();

	//
	// Configure IRQs
	//

	// turn off all interrupts
	INTERRUPTS->DISABLE[0] = 0;
	INTERRUPTS->DISABLE[1] = 0;
	INTERRUPTS->DISABLE_BASIC = 0;

	INTERRUPTS->ENABLE[0] |= (1 << 1); // set bit 1, enabling System Timer channel 1

	SYSTIMER->C1 = SYSTIMER->CLO + 1000000; // delay for some point in the future

	// set GPIO 20 to output
	GPIO->FSEL[2] &= ~(0b111);
	GPIO->FSEL[2] |= (0b001);

	GPIO->SET[0] |= (1 << 20);

	uint32_t el = get_el();

	bool on = false;

	while (true) {
		printf("current EL is: %d\n", el);

		if (on) {
			GPIO->CLEAR[0] |= (1 << 20);
		} else {
			GPIO->SET[0] |= (1 << 20);
		}

		on = !on;

		char c = uart_getc();
		if (c == '\r') {
			uart_putc('\n');
		}

		uart_putc(c);

		asm volatile("svc 42");
	}
}
