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
