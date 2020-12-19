#pragma once

#include <stddef.h>
#include <stdint.h>

//
// Raspberry Pi 3 Config
//
// TODO(harrison): move to rpi3.h that includes this file?
#define MMIO_BASE       0x3F000000

//
// Meat and potatoes
//

struct GPIO_Type {
	volatile uint32_t FSEL[6];
	uint32_t reserved1;
	volatile uint32_t SET[2];
	uint32_t reserved2;
	volatile uint32_t CLEAR[2];
	uint32_t reserved3;
	volatile uint32_t LEVEL[2];
	uint32_t reserved4;
	volatile uint32_t EVENT[2];
	uint32_t reserved5;
	volatile uint32_t RISING[2];
	uint32_t reserved6;
	volatile uint32_t FALLING[2];
	uint32_t reserved7;
	volatile uint32_t HIGH[2];
	uint32_t reserved8;
	volatile uint32_t LOW[2];
	uint32_t reserved9;
	volatile uint32_t ASYNC_RISING[2];
	uint32_t reserved10;
	volatile uint32_t ASYNC_FALLING[2];
	uint32_t reserved11;
	volatile uint32_t UD;
	volatile uint32_t UDCLK[2];
};

struct AUX_Type {
	volatile uint32_t IRQ;
	volatile uint32_t ENABLE;
};

struct MiniUART_Type {
	volatile uint32_t IO;
	volatile uint32_t IER;
	volatile uint32_t IIR;
	volatile uint32_t LCR;
	volatile uint32_t MCR;
	volatile uint32_t LSR;
	volatile uint32_t MSR;
	volatile uint32_t SCRATCH;
	volatile uint32_t CNTL;
	volatile uint32_t STAT;
	volatile uint32_t BAUD;
};

#define GPIO ((struct GPIO_Type*) (MMIO_BASE + 0x00200000))
#define AUX ((struct AUX_Type*) (MMIO_BASE + 0x00215000))
#define UART1 ((struct MiniUART_Type*) (MMIO_BASE + 0x00215040))
