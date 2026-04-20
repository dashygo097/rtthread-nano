TARGET ?= demu-virt-riscv32
ifeq ($(TARGET), demu-virt-riscv32)
  BSP_DIR   = bsp/demu-virt-riscv32
  CPU_DIR   = libcpu/risc-v/common
  MARCH     = rv32im_zicsr
  EXTRA_ASM = libcpu/risc-v/rv32m1/interrupt_gcc.S

else ifeq ($(TARGET), qemu-virt-riscv32)
  BSP_DIR   = bsp/qemu-virt-riscv32
  CPU_DIR   = libcpu/risc-v/common
  MARCH     = rv32im_zicsr
  EXTRA_ASM = libcpu/risc-v/rv32m1/interrupt_gcc.S

else
  $(error Unknown TARGET='$(TARGET)'. Valid values: demu-virt-riscv32, qemu-virt-riscv32)
endif

CROSS   ?= riscv64-unknown-elf
CC       = $(CROSS)-gcc
OBJCOPY  = $(CROSS)-objcopy
OBJDUMP  = $(CROSS)-objdump

BUILD_DIR = build

CFLAGS  = -march=$(MARCH) -mabi=ilp32 -mcmodel=medany \
          -ffunction-sections -fdata-sections          \
          -fno-common -fno-builtin -ffreestanding      \
          -Os -g -Wall

LDFLAGS = -T $(BSP_DIR)/linker.ld  \
          -nostartfiles             \
          -nostdlib                 \
          -Wl,--gc-sections         \
          -Wl,-Map=$(BUILD_DIR)/rtthread.map

INC_DIR = -I$(BSP_DIR) -Iinclude -I$(CPU_DIR)

C_SRCS = \
  $(BSP_DIR)/board.c        \
  $(BSP_DIR)/main.c         \
  $(CPU_DIR)/cpuport.c      \
  src/clock.c               \
  src/components.c          \
  src/idle.c                \
  src/ipc.c                 \
  src/irq.c                 \
  src/kservice.c            \
  src/mem.c                 \
  src/object.c              \
  src/scheduler.c           \
  src/thread.c              \
  src/timer.c

ASM_SRCS = \
  $(BSP_DIR)/startup.S      \
  $(CPU_DIR)/context_gcc.S  \
  $(EXTRA_ASM)

C_OBJS   = $(patsubst %.c,  $(BUILD_DIR)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.S,  $(BUILD_DIR)/%.o, $(ASM_SRCS))
OBJS     = $(ASM_OBJS) $(C_OBJS)

TARGET_ELF = $(BUILD_DIR)/rtthread.elf
TARGET_BIN = $(BUILD_DIR)/rtthread.bin
TARGET_DIS = $(BUILD_DIR)/rtthread.dis

.PHONY: all clean info

all: $(TARGET_ELF) $(TARGET_BIN) $(TARGET_DIS)
	@echo ""
	@echo "  TARGET : $(TARGET)"
	@echo "  BSP    : $(BSP_DIR)"
	@echo "  CPU    : $(CPU_DIR)"
	@echo "  ELF    : $(TARGET_ELF)"
	@echo "  BIN    : $(TARGET_BIN)"
	@echo ""

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIR) -x assembler-with-cpp -c $< -o $@

$(TARGET_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIR) \
	    -Wl,--start-group $(OBJS) -lgcc -Wl,--end-group \
	    $(LDFLAGS) -o $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET_DIS): $(TARGET_ELF)
	$(OBJDUMP) -D $< > $@

clean:
	rm -rf $(BUILD_DIR)

info:
	@echo "TARGET  = $(TARGET)"
	@echo "BSP_DIR = $(BSP_DIR)"
	@echo "CPU_DIR = $(CPU_DIR)"
	@echo "MARCH   = $(MARCH)"
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "C_SRCS  ="
	@$(foreach s,$(C_SRCS),  echo "  $(s)";)
	@echo "ASM_SRCS="
	@$(foreach s,$(ASM_SRCS),echo "  $(s)";)
