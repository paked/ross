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

void bootloaded(void) {
	uart_init();

	printf("hello world\n");

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
	}
}
