OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    IMEM (rx)  : ORIGIN = 0x80000000, LENGTH = 256K
    DMEM (rwx) : ORIGIN = 0x80040000, LENGTH = 256K
}

SECTIONS
{
    .text : {
        KEEP(*(.text.entry))
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } > IMEM

    .rodata : {
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > IMEM

    .data : {
        _data_start = .;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
        . = ALIGN(4);
        _data_end = .;
    } > DMEM AT > IMEM

    _data_load = LOADADDR(.data);

    .bss : {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > DMEM

    __stack = ORIGIN(DMEM) + LENGTH(DMEM);

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.riscv.attributes)
    }
}
