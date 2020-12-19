#include <stdbool.h>
#include <stdint.h>

#include "bcm2385.h"
#include "uart.h"

void bootloaded(void) {
	uart_init();

	// set GPIO 20 to output
	GPIO->FSEL[2] &= ~(0b111);
	GPIO->FSEL[2] |= (0b001);

	GPIO->SET[0] |= (1 << 20);

	bool on = true;

	while (true) {
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
