.section ".text.boot"

.global _start

_start:
	ldr     x1, =_start
	mov     sp, x1

	mov fp, #0  // I don't think necessary.

	// clear bss
	ldr     x1, =__bss_start
	ldr     w2, =__bss_size
3:	cbz     w2, 4f
	str     xzr, [x1], #8
	sub     w2, w2, #1
	cbnz    w2, 3b

4:
	bl bootloaded

1:	wfe
	b 1b
