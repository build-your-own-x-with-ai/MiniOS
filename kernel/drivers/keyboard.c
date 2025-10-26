#include "types.h"
#include "keyboard.h"
#include "idt.h"
#include "io.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define BUFFER_SIZE 256

// 环形缓冲区
static char key_buffer[BUFFER_SIZE];
static int buffer_start = 0;
static int buffer_end = 0;

// 美式键盘扫描码到 ASCII 映射表（简化版）
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

// 添加字符到缓冲区
static void buffer_put(char c) {
    int next = (buffer_end + 1) % BUFFER_SIZE;
    if (next != buffer_start) {
        key_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

// 从缓冲区获取字符
static char buffer_get(void) {
    if (buffer_start == buffer_end) {
        return 0;
    }
    
    char c = key_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % BUFFER_SIZE;
    return c;
}

// 键盘中断处理函数
void keyboard_handler(struct interrupt_frame* frame) {
    (void)frame;
    
    // 读取扫描码
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // 只处理按键按下事件（扫描码 < 0x80）
    if (scancode < 0x80) {
        if (scancode < sizeof(scancode_to_ascii)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                buffer_put(c);
            }
        }
    }
}

void keyboard_init(void) {
    // 注册键盘中断处理函数（IRQ1 = INT 33）
    register_interrupt_handler(33, keyboard_handler);
}

char keyboard_getchar(void) {
    return buffer_get();
}

int keyboard_available(void) {
    return buffer_start != buffer_end;
}
