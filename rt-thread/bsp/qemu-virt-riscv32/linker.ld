OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 16M
}

SECTIONS
{
    .text : {
        KEEP(*(.text.entry))
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } > RAM

    .rodata : {
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > RAM

    .data : {
        *(.data)
        *(.data*)
        . = ALIGN(4);
    } > RAM

    .bss : {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    __stack = ORIGIN(RAM) + LENGTH(RAM);

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
