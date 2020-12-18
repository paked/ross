BootloadedOffset = 1024;

SECTIONS
{
    . = 0x80000;

    .text : {
        KEEP(*(.boot));
        *(.text*)
    }

    .rodata ALIGN(16) : {
        *(.rodata*)
    }

    .data ALIGN(16) : {
        *(.data*)
    }

    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    _end = .;

    . = 0x80000 + BootloadedOffset;
}

__bss_size = (__bss_end - __bss_start)>>3;
