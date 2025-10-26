; 中断服务例程 (ISR) 汇编代码

global idt_flush
extern idtp

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

; ISR 宏 - 无错误码
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push 0           ; 压入虚拟错误码
    push %1          ; 压入中断号
    jmp isr_common_stub
%endmacro

; ISR 宏 - 有错误码
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push %1          ; 压入中断号
    jmp isr_common_stub
%endmacro

; IRQ 宏
%macro IRQ 2
global irq%1
irq%1:
    cli
    push 0
    push %2
    jmp irq_common_stub
%endmacro

; CPU 异常 (0-31)
ISR_NOERRCODE 0   ; 除零错误
ISR_NOERRCODE 1   ; 调试
ISR_NOERRCODE 2   ; 非屏蔽中断
ISR_NOERRCODE 3   ; 断点
ISR_NOERRCODE 4   ; 溢出
ISR_NOERRCODE 5   ; 边界检查
ISR_NOERRCODE 6   ; 非法指令
ISR_NOERRCODE 7   ; 设备不可用
ISR_ERRCODE   8   ; 双重错误
ISR_NOERRCODE 9   ; 协处理器段超限
ISR_ERRCODE   10  ; 无效 TSS
ISR_ERRCODE   11  ; 段不存在
ISR_ERRCODE   12  ; 栈段错误
ISR_ERRCODE   13  ; 通用保护错误
ISR_ERRCODE   14  ; 页错误
ISR_NOERRCODE 15  ; 保留
ISR_NOERRCODE 16  ; 浮点错误
ISR_ERRCODE   17  ; 对齐检查
ISR_NOERRCODE 18  ; 机器检查
ISR_NOERRCODE 19  ; SIMD 浮点异常
ISR_NOERRCODE 20  ; 虚拟化异常
ISR_NOERRCODE 21  ; 保留
ISR_NOERRCODE 22  ; 保留
ISR_NOERRCODE 23  ; 保留
ISR_NOERRCODE 24  ; 保留
ISR_NOERRCODE 25  ; 保留
ISR_NOERRCODE 26  ; 保留
ISR_NOERRCODE 27  ; 保留
ISR_NOERRCODE 28  ; 保留
ISR_NOERRCODE 29  ; 保留
ISR_NOERRCODE 30  ; 保留
ISR_NOERRCODE 31  ; 保留

; 硬件中断 (IRQ 0-15 映射到 INT 32-47)
IRQ 0, 32    ; 定时器
IRQ 1, 33    ; 键盘
IRQ 2, 34    ; 级联
IRQ 3, 35    ; COM2
IRQ 4, 36    ; COM1
IRQ 5, 37    ; LPT2
IRQ 6, 38    ; 软盘
IRQ 7, 39    ; LPT1
IRQ 8, 40    ; CMOS 实时时钟
IRQ 9, 41    ; 空闲
IRQ 10, 42   ; 空闲
IRQ 11, 43   ; 空闲
IRQ 12, 44   ; PS/2 鼠标
IRQ 13, 45   ; FPU
IRQ 14, 46   ; 主 ATA
IRQ 15, 47   ; 从 ATA

; 系统调用 (INT 0x80 = 128)
ISR_NOERRCODE 128

extern isr_handler

; 通用 ISR 存根
isr_common_stub:
    pusha              ; 保存所有通用寄存器
    
    mov ax, ds
    push eax           ; 保存数据段
    
    mov ax, 0x10       ; 加载内核数据段
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp           ; 传递栈指针（中断帧）
    call isr_handler
    add esp, 4
    
    pop eax            ; 恢复数据段
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa               ; 恢复所有通用寄存器
    add esp, 8         ; 清理错误码和中断号
    sti
    iret               ; 从中断返回

extern irq_handler

; 通用 IRQ 存根
irq_common_stub:
    pusha
    
    mov ax, ds
    push eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp
    call irq_handler
    add esp, 4
    
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa
    add esp, 8
    sti
    iret
