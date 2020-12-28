#pragma once

#include <stdint.h>

struct context {
	volatile uint64_t spsr;
	volatile uint64_t elr;

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
	int64_t pid;
	struct context *context;
};
