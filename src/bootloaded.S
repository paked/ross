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
3:
	wfe

	b 3b

.macro ctx_save
	stp x0, x1, [sp, #-16]!
	stp x2, x3, [sp, #-16]!
	stp x4, x5, [sp, #-16]!
	stp x6, x7, [sp, #-16]!
	stp x8, x9, [sp, #-16]!
	stp x10, x11, [sp, #-16]!
	stp x12, x13, [sp, #-16]!
	stp x14, x15, [sp, #-16]!
	stp x16, x17, [sp, #-16]!
	stp x18, x19, [sp, #-16]!
	stp x20, x21, [sp, #-16]!
	stp x22, x23, [sp, #-16]!
	stp x24, x25, [sp, #-16]!
	stp x26, x27, [sp, #-16]!
	stp x28, x29, [sp, #-16]!
	stp x30, xzr, [sp, #-16]!

	mrs x0, SPSR_EL1
	mrs x1, ELR_EL1
	stp x0, x1, [sp, #-16]!

	ldr x1, =next_context

	mov x0, sp
	str x0, [x1]
.endm

.macro ctx_restore
	ldr x0, =next_context
	ldr x0, [x0]
	mov sp, x0

	ldp x0, x1, [sp], #16
	msr SPSR_EL1, x0
	msr ELR_EL1, x1

	ldp x30, xzr, [sp], #16
	ldp x28, x29, [sp], #16
	ldp x26, x27, [sp], #16
	ldp x24, x25, [sp], #16
	ldp x22, x23, [sp], #16
	ldp x20, x21, [sp], #16
	ldp x18, x19, [sp], #16
	ldp x16, x17, [sp], #16
	ldp x14, x15, [sp], #16
	ldp x12, x13, [sp], #16
	ldp x10, x11, [sp], #16
	ldp x8, x9, [sp], #16
	ldp x6, x7, [sp], #16
	ldp x4, x5, [sp], #16
	ldp x2, x3, [sp], #16
	ldp x0, x1, [sp], #16
.endm

.align 11
vectors:
	.align 7
	mov x0, 0
	b die // Synchronous EL1t
	.align 7
	mov x0, 1
	b die // IRQ EL1t
	.align 7
	mov x0, 2
	b die // FIQ EL1t
	.align 7
	mov x0, 3
	b die // Error EL1t

	.align 7
	b sync_el1h
	.align 7
	b irq_el1h
	.align 7
	mov x0, 6
	b die // FIQ EL1h
	.align 7
	mov x0, 7
	b die // Error EL1h

	.align 7
	mov x0, 8
	b die // Synchronous EL0 (64 bit)
	.align 7
	mov x0, 9
	b die // IRQ EL0 (64 bit)
	.align 7
	mov x0, 10
	b die // FIQ EL0 (64 bit)
	.align 7
	mov x0, 11
	b die // Error EL0 (64 bit)

	.align 7
	mov x0, 12
	b die // Synchronous EL0 (32 bit)
	.align 7
	mov x0, 13
	b die // IRQ EL0 (32 bit)
	.align 7
	mov x0, 14
	b die // FIQ EL0 (32 bit)
	.align 7
	mov x0, 15
	b die // Error EL0 (32 bit)

sync_el1h:
	ctx_save

	mrs x0, ESR_EL1
	bl exception_synchronous

	ctx_restore

	eret

irq_el1h:
	ctx_save

	mov x0, sp
	bl exception_irq

	ctx_restore

	eret
