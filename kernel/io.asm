; I/O 端口操作函数

global inb
global outb

; uint8_t inb(uint16_t port)
inb:
    mov dx, [esp+4]  ; port
    xor eax, eax
    in al, dx
    ret

; void outb(uint16_t port, uint8_t val)
outb:
    mov dx, [esp+4]  ; port
    mov al, [esp+8]  ; val
    out dx, al
    ret
