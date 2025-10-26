; Mini OS 引导代码
; 使用 Multiboot 规范

; Multiboot 头部常量
MULTIBOOT_MAGIC        equ 0x1BADB002
MULTIBOOT_ALIGN        equ 1 << 0
MULTIBOOT_MEMINFO      equ 1 << 1
MULTIBOOT_FLAGS        equ MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO
MULTIBOOT_CHECKSUM     equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; 栈大小
STACK_SIZE equ 16384  ; 16KB

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb STACK_SIZE
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; 设置栈指针
    mov esp, stack_top
    
    ; 重置 EFLAGS
    push 0
    popf
    
    ; 将 Multiboot 信息传递给内核
    ; EAX 包含魔数
    ; EBX 包含 Multiboot 信息结构的物理地址
    push ebx
    push eax
    
    ; 调用 C 内核入口
    call kernel_main
    
    ; 如果 kernel_main 返回，进入无限循环
    cli
.hang:
    hlt
    jmp .hang
