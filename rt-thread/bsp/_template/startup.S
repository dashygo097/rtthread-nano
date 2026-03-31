.section .text.entry, "ax"
.globl _start

_start:
    csrw mstatus, zero
    csrw mie, zero

    lui sp, 0x80040
    
    li t0, -16
    and sp, sp, t0
    
    mv fp, zero

    la t0, _sbss
    la t1, _ebss
    beq t0, t1, bss_clear_done
bss_clear_loop:
    sb zero, 0(t0)
    addi t0, t0, 1
    bltu t0, t1, bss_clear_loop
bss_clear_done:

    jal ra, entry

halt:
    j halt
