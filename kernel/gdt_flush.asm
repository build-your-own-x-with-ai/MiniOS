; GDT 加载汇编代码

global gdt_flush
extern gp

gdt_flush:
    mov eax, [esp+4]  ; 获取 GDT 指针参数
    lgdt [eax]        ; 加载 GDT
    
    ; 重新加载段寄存器
    mov ax, 0x10      ; 0x10 是内核数据段的偏移
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; 远跳转到新的代码段
    jmp 0x08:.flush   ; 0x08 是内核代码段的偏移
.flush:
    ret
