.section .text.entry, "ax"
.globl _start
.align 2

_start:
    csrw    mstatus, zero
    csrw    mie,     zero

    la      sp, __stack

    la      a0, _data_load
    la      a1, _data_start
    la      a2, _data_end
    bgeu    a1, a2, data_copy_done

data_copy_loop:
    lw      t0, 0(a0)
    sw      t0, 0(a1)
    addi    a0, a0, 4
    addi    a1, a1, 4
    bltu    a1, a2, data_copy_loop

data_copy_done:
    la      a0, _sbss
    la      a1, _ebss
    bgeu    a0, a1, bss_clear_done

bss_clear_loop:
    sw      zero, 0(a0)
    addi    a0,   a0, 4
    bltu    a0,   a1, bss_clear_loop
bss_clear_done:
    call    entry

halt:
    j       halt
