#include <kernel.h>

#include <stdbool.h>

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
	printf("addr/sp=%x\n", context);
}

#define MAX_PROCESS_COUNT (8)
struct process processes[MAX_PROCESS_COUNT] = {0};

struct process* process_current = 0;
volatile struct context* next_context = 0;

uint64_t scheduler_current_index = 0;

int64_t pid_next = 1;

// TODO(harrison): set up memory allocation so we can allocate a stack at run
// time
int64_t process_create(void (*handler)(void), char* stack, size_t size) {
	struct process *process = 0;

	for (int64_t i = 0; i < MAX_PROCESS_COUNT; i++) {
		if (processes[i].pid != 0) {
			continue;
		}

		process = &processes[i];
		process->pid = pid_next++;

		break;
	}

	if (process == 0) {
		printf("too many concurrent processes\n");

		return -1;
	}

	char* sp_top = stack + (size-sizeof(struct context));
	uint64_t offs = (uint64_t) sp_top % 16; // sp gotta be 16 byte aligned
	sp_top -= offs;

	process->context = (struct context*) sp_top;
	process->context->elr = (uint64_t) handler;
	process->context->spsr = 0b0101; // set mode to el1h

	return process->pid;
}

void schedule() {
	if (process_current != 0) {
		process_current->context = (struct context*) next_context;
	}

	struct process *next_process = 0;

	for (uint32_t i = 0; i < MAX_PROCESS_COUNT; i++) {
		uint32_t real_index = (scheduler_current_index+i+1) % MAX_PROCESS_COUNT;
		struct process *process = &processes[real_index];

		if (process->pid == 0) {
			continue;
		}

		next_process = process;
		scheduler_current_index = real_index;

		printf("chose %d %x %d\n", process->pid, process->context, scheduler_current_index);


		break;
	}

	process_current = next_process;
	next_context = process_current->context;
}

void exception_synchronous_handle_svc(uint32_t imm) {
	switch (imm) {
		case 1: // yield to scheduler
			{
				schedule();
			} break;
		case 42: // test
			{
				printf("42 yolo!\n");
			} break;
	}
}

void exception_synchronous(uint64_t esr) {
	uint32_t smol_esr = (uint32_t) esr;

	uint32_t ec = (smol_esr >> 26) & (0b111111);

	switch (ec) {
		case 0b010101: // SVC
			{
				uint16_t imm = smol_esr & 0xFFFF;
				printf("got svc %d\n", imm);
				exception_synchronous_handle_svc(imm);
			} break;
		default:
			printf("unknown synchronous exception syndrome! cannot recover. %d\n", ec);

			while(true);
	}
}

void exception_irq_handle_systimer(struct context* context) {
	schedule();

	SYSTIMER->C1 = SYSTIMER->CLO + 5000000; // delay for some point in the future
}

void exception_irq(void* ptr) {
	printf("got irq exception\n");

	if (INTERRUPTS->PENDING[0] & (1 << 1)) {
		exception_irq_handle_systimer((struct context*) ptr);
		SYSTIMER->CS |= (1 << 1);
	}
}

void die(uint64_t r) {
	printf("unhandled exception called! cannot recover. %d\n", r);

	while (true);
}

void process_fn_1() {
	while (true) {
		printf("goodbye world\n");
	}
}

void process_fn_2() {
	while (true) {
		printf("hello world\n");
	}
}


char mem[2048 * 8] = {0};
char mem2[2048 * 8] = {0};

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

	// set GPIO 20 to output
	GPIO->FSEL[2] &= ~(0b111);
	GPIO->FSEL[2] |= (0b001);

	GPIO->SET[0] |= (1 << 20);

	//
	// Initialise process
	//

	process_create(&process_fn_1, mem, sizeof(mem));
	process_create(&process_fn_2, mem2, sizeof(mem2));

	uart_putc(uart_getc());

	SYSTIMER->C1 = SYSTIMER->CLO + 5000000; // delay for some point in the future

	asm volatile("svc 1");

	// NOTE(harrison): should never end up here
	while (true);
}
