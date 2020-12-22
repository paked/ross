.section ".text.boot"

.global _start

_start:
	// set up stack pointer
	ldr x1, =_start
	mov sp, x1

	mov fp, #0  // I don't think necessary.

	//
	// switch to EL1
	//

	mov x0, #(1 << 31) // set RW bit (make EL1 use aarch64)
	msr HCR_EL2, x0

	mov x0, #0b0101 // switch to EL1h (EL1h, which means we keep the stack seperate from EL0)
	msr SPSR_EL2, x0

	adr x0, el1_entry // set return address to the continuing code
	msr ELR_EL2, x0

	adr x0, vectors
	msr VBAR_EL1, x0 // set vector table

	eret // zoom

el1_entry:
	// set up stack pointer
	ldr x1, =_start
	mov sp, x1

	// clear bss
	ldr x1, =__bss_start
	ldr w2, =__bss_size
1:	cbz w2, 2f
	str xzr, [x1], #8
	sub w2, w2, #1
	cbnz w2, 1b
2:

	bl bootloaded

3:	wfe
	b 3b


.macro	kernel_entry
	sub	sp, sp, #256
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	str	x30, [sp, #16 * 15] 
.endm

.macro	kernel_exit
	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30, [sp, #16 * 15] 
	add	sp, sp, #256
.endm

.align 11
vectors:
	.align 7
	b die // Synchronous EL1t
	.align 7
	b die // IRQ EL1t
	.align 7
	b die // FIQ EL1t
	.align 7
	b die // Error EL1t

	.align 7
	b sync_el1h
	.align 7
	b die // IRQ EL1h
	.align 7
	b die // FIQ EL1h
	.align 7
	b die // Error EL1h

	.align 7
	b die // Synchronous EL0 (64 bit)
	.align 7
	b die // IRQ EL0 (64 bit)
	.align 7
	b die // FIQ EL0 (64 bit)
	.align 7
	b die // Error EL0 (64 bit)

	.align 7
	b die // Synchronous EL0 (32 bit)
	.align 7
	b die // IRQ EL0 (32 bit)
	.align 7
	b die // FIQ EL0 (32 bit)
	.align 7
	b die // Error EL0 (32 bit)

sync_el1h:
	kernel_entry

	mrs x0, ESR_EL1
	bl synchronous_exception

	kernel_exit

	eret
