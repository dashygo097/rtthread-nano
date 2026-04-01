# start.S - Minimal RISC-V startup code
.globl _start       

_start:
    csrw mstatus, zero
    csrw mie, zero

    lui sp, 0x80004
    mv fp, zero

    jal ra, entry 

    j .

