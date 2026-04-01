# Demu Virt RISC-V 32 BSP Makefile for RT-Thread Nano
BSP_DIR   = bsp/qemu-virt-riscv32
BUILD_DIR = build
CPU_DIR   = libcpu/risc-v/common

CC      = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump

CFLAGS  = -march=rv32i_zicsr -mabi=ilp32 -mcmodel=medany -g -Wall
LDFLAGS = -T $(BSP_DIR)/linker.ld -nostdlib -lc -lgcc

INC_DIR = -I$(BSP_DIR) -Iinclude -I$(CPU_DIR)

C_SRCS = \
	$(BSP_DIR)/board.c \
	$(BSP_DIR)/main.c \
	$(CPU_DIR)/cpuport.c \
	src/clock.c \
	src/components.c \
	src/idle.c \
	src/ipc.c \
	src/irq.c \
	src/kservice.c \
	src/mem.c \
	src/object.c \
	src/scheduler.c \
	src/thread.c \
	src/timer.c

ASM_SRCS = \
	$(BSP_DIR)/startup.S \
	$(CPU_DIR)/context_gcc.S \
	libcpu/risc-v/rv32m1/interrupt_gcc.S

C_OBJS   = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.S, $(BUILD_DIR)/%.o, $(ASM_SRCS))
OBJS     = $(ASM_OBJS) $(C_OBJS)

TARGET_ELF = $(BUILD_DIR)/rtthread.elf
TARGET_BIN = $(BUILD_DIR)/rtthread.bin
TARGET_DIS = $(BUILD_DIR)/rtthread.dis

all: $(TARGET_ELF) $(TARGET_BIN) $(TARGET_DIS)
	@echo "Build completed! Output in $(BUILD_DIR)/"

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

$(TARGET_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIR) $(OBJS) $(LDFLAGS) -o $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET_DIS): $(TARGET_ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
