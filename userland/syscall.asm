; 用户程序系统调用汇编实现

global syscall1
global syscall3

syscall1:
    mov eax, [esp+4]   ; 系统调用号
    mov ebx, [esp+8]   ; 参数 1
    int 0x80
    ret

syscall3:
    mov eax, [esp+4]   ; 系统调用号
    mov ebx, [esp+8]   ; 参数 1
    mov ecx, [esp+12]  ; 参数 2
    mov edx, [esp+16]  ; 参数 3
    int 0x80
    ret
