; 辅助函数 - 停止 CPU 和读取/设置控制寄存器

global cpu_halt
global read_cr2
global load_page_directory
global enable_paging

cpu_halt:
    cli           ; 禁用中断
.loop:
    hlt           ; 停止 CPU
    jmp .loop     ; 如果被中断唤醒，继续循环

read_cr2:
    mov eax, cr2  ; 读取 CR2 寄存器到 EAX
    ret

load_page_directory:
    mov eax, [esp+4]  ; 获取页目录地址参数
    mov cr3, eax      ; 加载到 CR3
    ret

enable_paging:
    mov eax, cr0      ; 读取 CR0
    or eax, 0x80000000 ; 设置 PG 位
    mov cr0, eax      ; 写回 CR0
    ret
