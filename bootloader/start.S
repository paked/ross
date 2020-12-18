.section ".text.boot"

.global _start

_start:
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      kernel_main

    // for failsafe, halt this core
1:  wfe
    b       1b


.global BRANCH_TO
BRANCH_TO:
    mov x30, x0
    ret
