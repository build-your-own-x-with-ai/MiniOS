#include "types.h"
#include "vga.h"

// 简单的用户程序演示（在内核空间运行，但模拟用户程序行为）

void run_user_demo(void) {
    terminal_writestring("\n=== User Program Demo ===\n\n");
    
    // 演示 1: Hello World
    terminal_writestring("Demo 1: Hello World\n");
    terminal_writestring("  Output: Hello, Mini OS!\n");
    terminal_writestring("  This is a user program.\n\n");
    
    // 演示 2: 计算
    terminal_writestring("Demo 2: Calculations\n");
    terminal_writestring("  Computing sum of 1 to 10...\n");
    
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;
    }
    
    terminal_writestring("  Result: ");
    char buf[16];
    int idx = 0;
    if (sum == 0) {
        buf[idx++] = '0';
    } else {
        int temp = sum;
        int digits = 0;
        while (temp > 0) {
            temp /= 10;
            digits++;
        }
        for (int j = digits - 1; j >= 0; j--) {
            buf[j] = '0' + (sum % 10);
            sum /= 10;
        }
        idx = digits;
    }
    buf[idx] = '\0';
    terminal_writestring(buf);
    terminal_writestring(" (expected: 55)\n\n");
    
    // 演示 3: 字符串操作
    terminal_writestring("Demo 3: String Operations\n");
    terminal_writestring("  Alphabet: ");
    
    char alphabet[27];
    for (int i = 0; i < 26; i++) {
        alphabet[i] = 'A' + i;
    }
    alphabet[26] = '\0';
    terminal_writestring(alphabet);
    terminal_writestring("\n\n");
    
    // 演示 4: 内存操作
    terminal_writestring("Demo 4: Memory Operations\n");
    terminal_writestring("  Allocating and using stack memory...\n");
    
    int numbers[10];
    for (int i = 0; i < 10; i++) {
        numbers[i] = i * i;
    }
    
    terminal_writestring("  Squares: ");
    for (int i = 0; i < 10; i++) {
        int num = numbers[i];
        char num_buf[16];
        int n_idx = 0;
        
        if (num == 0) {
            num_buf[n_idx++] = '0';
        } else {
            int temp = num;
            int digits = 0;
            while (temp > 0) {
                temp /= 10;
                digits++;
            }
            for (int j = digits - 1; j >= 0; j--) {
                num_buf[j] = '0' + (num % 10);
                num /= 10;
            }
            n_idx = digits;
        }
        num_buf[n_idx] = '\0';
        
        terminal_writestring(num_buf);
        if (i < 9) terminal_writestring(", ");
    }
    terminal_writestring("\n\n");
    
    terminal_writestring("=== Demo Complete ===\n");
    terminal_writestring("All user program features demonstrated!\n\n");
}
