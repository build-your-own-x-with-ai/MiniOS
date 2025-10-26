# Mini OS Makefile

# 工具链配置
# 对于 macOS，使用交叉编译工具链：
# CC = i386-elf-gcc
# LD = i386-elf-ld
AS = nasm
CC = gcc
LD = ld
GRUB_MKRESCUE = grub-mkrescue

# 编译标志
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra -c
LDFLAGS = -m elf_i386 -T linker.ld

# 目录
BOOT_DIR = boot
KERNEL_DIR = kernel
INCLUDE_DIR = include
USERLAND_DIR = userland
BUILD_DIR = build
ISO_DIR = iso

# 源文件
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_C_SRC = $(wildcard $(KERNEL_DIR)/*.c) \
               $(wildcard $(KERNEL_DIR)/memory/*.c) \
               $(wildcard $(KERNEL_DIR)/process/*.c) \
               $(wildcard $(KERNEL_DIR)/fs/*.c) \
               $(wildcard $(KERNEL_DIR)/interrupt/*.c) \
               $(wildcard $(KERNEL_DIR)/drivers/*.c) \
               $(KERNEL_DIR)/user_demo.c
KERNEL_ASM_SRC = $(wildcard $(KERNEL_DIR)/*.asm) \
                 $(wildcard $(KERNEL_DIR)/interrupt/*.asm)

# 目标文件
BOOT_OBJ = $(BUILD_DIR)/$(BOOT_DIR)/boot.o
KERNEL_C_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SRC))
KERNEL_ASM_OBJ = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SRC))
KERNEL_OBJ = $(BOOT_OBJ) $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ) $(BUILD_DIR)/kernel/gdt_flush.o $(BUILD_DIR)/kernel/panic.o $(BUILD_DIR)/kernel/halt.o $(BUILD_DIR)/kernel/io.o

# 用户程序
USERLAND_PROGRAMS = hello
USERLAND_BIN = $(patsubst %,$(BUILD_DIR)/userland/%,$(USERLAND_PROGRAMS))
USERLAND_OBJ = $(BUILD_DIR)/userland_embed.o

# 最终目标
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_FILE = mini-os.iso

.PHONY: all clean iso run dirs kernel-only

all: dirs $(KERNEL_BIN)

# 仅编译内核（跳过用户程序，适用于 macOS）
kernel-only: dirs $(KERNEL_OBJ)
	@echo "内核对象文件编译完成！"
	@echo ""
	@echo "注意：由于 macOS 限制，无法完成链接步骤。"
	@echo "要完成构建，请使用以下方法之一："
	@echo "  1. Docker:  ./build-docker.sh"
	@echo "  2. 交叉编译: 安装 i386-elf-gcc"
	@echo "  3. Linux VM: 在虚拟机中构建"
	@echo ""
	@echo "查看 QUICKSTART.md 了解详情"

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(BOOT_DIR)
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/memory
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/process
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/fs
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/interrupt
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/$(USERLAND_DIR)

# 编译引导代码
$(BUILD_DIR)/$(BOOT_DIR)/%.o: $(BOOT_DIR)/%.asm
	$(AS) $(ASFLAGS) $< -o $@

# 编译内核 C 代码
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $< -o $@

# 编译内核汇编代码
$(BUILD_DIR)/%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# 链接内核
$(KERNEL_BIN): $(KERNEL_OBJ) $(USERLAND_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# 编译用户程序汇编
$(BUILD_DIR)/userland/syscall.o: $(USERLAND_DIR)/syscall.asm
	$(AS) $(ASFLAGS) $< -o $@

# 编译用户程序
$(BUILD_DIR)/userland/%: $(USERLAND_DIR)/%.c $(USERLAND_DIR)/lib.c $(BUILD_DIR)/userland/syscall.o
	$(CC) -m32 -nostdlib -static -I$(INCLUDE_DIR) -o $@ $(USERLAND_DIR)/lib.c $< $(BUILD_DIR)/userland/syscall.o -Wl,--entry=_start

# 将用户程序嵌入到内核
$(USERLAND_OBJ): $(USERLAND_BIN)
	@echo "Embedding user programs..."
	@cd $(BUILD_DIR)/userland && objcopy -I binary -O elf32-i386 -B i386 hello hello.o
	$(LD) -m elf_i386 -r -o $@ $(BUILD_DIR)/userland/hello.o

# 创建 ISO 镜像
iso: $(KERNEL_BIN)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	@cp grub.cfg $(ISO_DIR)/boot/grub/
	$(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR)
	@echo "ISO 镜像已创建: $(ISO_FILE)"

# 在 QEMU 中运行（用于快速测试）
run: iso
	qemu-system-i386 -cdrom $(ISO_FILE)

# 清理
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_FILE)

# 帮助信息
help:
	@echo "Mini OS 构建系统"
	@echo ""
	@echo "可用目标:"
	@echo "  all     - 编译内核（默认）"
	@echo "  iso     - 创建可引导 ISO 镜像"
	@echo "  run     - 在 QEMU 中运行"
	@echo "  clean   - 清理构建文件"
	@echo "  help    - 显示此帮助信息"
