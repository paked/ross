#include <stdbool.h>
#include <stdint.h>

#include "bcm2385.h"
#include "uart.h"
#include "util.h"

#define XPRINTF_IMPLEMENTATION
#include "xprintf.h"

#define printf(...) xprintf(&write, __VA_ARGS__)

struct context {
	volatile uint64_t sp;
	volatile uint64_t reserved;

	volatile uint64_t elr;
	volatile uint64_t spsr;

	volatile uint64_t x30;
	volatile uint64_t xzr;

	volatile uint64_t x28;
	volatile uint64_t x29;

	volatile uint64_t x26;
	volatile uint64_t x27;

	volatile uint64_t x24;
	volatile uint64_t x25;

	volatile uint64_t x22;
	volatile uint64_t x23;

	volatile uint64_t x20;
	volatile uint64_t x21;

	volatile uint64_t x18;
	volatile uint64_t x19;

	volatile uint64_t x16;
	volatile uint64_t x17;

	volatile uint64_t x14;
	volatile uint64_t x15;

	volatile uint64_t x12;
	volatile uint64_t x13;

	volatile uint64_t x10;
	volatile uint64_t x11;

	volatile uint64_t x8;
	volatile uint64_t x9;

	volatile uint64_t x6;
	volatile uint64_t x7;

	volatile uint64_t x4;
	volatile uint64_t x5;

	volatile uint64_t x2;
	volatile uint64_t x3;

	volatile uint64_t x0;
	volatile uint64_t x1;
};

struct process {
	struct context context;
};

uint32_t write(char* buf, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		if (buf[i] == '\n') {
			uart_putc('\r');
		}

		uart_putc(buf[i]);
	}

	return len;
}

void exception_synchronous(uint64_t esr) {
	uint32_t smol_esr = (uint32_t) esr;

	uint32_t ec = (smol_esr >> 26) & (0b111111);

	switch (ec) {
		case 0b010101: // SVC
			{
				uint32_t imm = smol_esr & 0xFF;

				printf("got svc %d\n", imm);
			} break;
		default:
			printf("unknown synchronous exception syndrome! cannot recover. %d\n", ec);

			while(true);
	}
}

void print_context(struct context *context) {
	// python3: print('\n'.join(["printf(\"x{i}=%x\", context->x{i})".format(i=x) for x in range(0, 30)]))

	printf("x0=%x\n", context->x0);
	printf("x1=%x\n", context->x1);
	printf("x2=%x\n", context->x2);
	printf("x3=%x\n", context->x3);
	printf("x4=%x\n", context->x4);
	printf("x5=%x\n", context->x5);
	printf("x6=%x\n", context->x6);
	printf("x7=%x\n", context->x7);
	printf("x8=%x\n", context->x8);
	printf("x9=%x\n", context->x9);
	printf("x10=%x\n", context->x10);
	printf("x11=%x\n", context->x11);
	printf("x12=%x\n", context->x12);
	printf("x13=%x\n", context->x13);
	printf("x14=%x\n", context->x14);
	printf("x15=%x\n", context->x15);
	printf("x16=%x\n", context->x16);
	printf("x17=%x\n", context->x17);
	printf("x18=%x\n", context->x18);
	printf("x19=%x\n", context->x19);
	printf("x20=%x\n", context->x20);
	printf("x21=%x\n", context->x21);
	printf("x22=%x\n", context->x22);
	printf("x23=%x\n", context->x23);
	printf("x24=%x\n", context->x24);
	printf("x25=%x\n", context->x25);
	printf("x26=%x\n", context->x26);
	printf("x27=%x\n", context->x27);
	printf("x28=%x\n", context->x28);
	printf("x29=%x\n", context->x29);
	printf("x30=%x\n", context->x30);

	printf("xzr=%x\n", context->xzr);
	printf("spsr=%x\n", context->spsr);
	printf("elr=%x\n", context->elr);
	printf("sp=%x\n", context->sp);
}

void exception_irq_handle_systimer(struct context* context) {
	print_context(context);

	SYSTIMER->C1 = SYSTIMER->CLO + 1000000; // delay for some point in the future
}

void exception_irq(void* ptr) {
	printf("got irq exception\n");

	if (INTERRUPTS->PENDING[0] & (1 << 1)) {
		exception_irq_handle_systimer((struct context*) ptr);
		SYSTIMER->CS |= (1 << 1);
	}
}

void die() {
	printf("unhandled exception called! cannot recover.\n");

	while (true);
}

void process_fn_1() {
	while (true) {
		uart_putc(uart_getc());
	}
}

extern void done(void);

void bootloaded(void) {
	uart_init();

	printf("hello world\n");

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

	done();

	/*
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
	*/

	//
	// Create process
	//

	// zero out all registers

	// set up pc, sp, lr

	//
	// Context switch
	//

	// set sp and lr correctly
	// branch to pc

	// while (true);
}
