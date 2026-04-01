.section .text.entry, "ax"
.globl _start

_start:
    csrw mstatus, zero
    csrw mie, zero

    la sp, __stack
    
    li t0, -16
    and sp, sp, t0
    
    mv fp, zero

    la t0, _sbss
    la t1, _ebss
bss_clear_loop:
    bge t0, t1, bss_clear_done
    sw zero, 0(t0)
    addi t0, t0, 4
    j bss_clear_loop
bss_clear_done:

    jal ra, entry

halt:
    j halt

