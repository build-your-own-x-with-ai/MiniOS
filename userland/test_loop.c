// 循环和计算测试程序

extern void puts(const char* str);
extern void exit(int code);

// 简单的整数转字符串
void print_number(int num) {
    char buf[12];
    int i = 0;
    
    if (num == 0) {
        buf[i++] = '0';
    } else {
        int temp = num;
        int digits = 0;
        while (temp > 0) {
            temp /= 10;
            digits++;
        }
        for (int j = digits - 1; j >= 0; j--) {
            buf[j] = '0' + (num % 10);
            num /= 10;
        }
        i = digits;
    }
    buf[i] = '\0';
    
    puts(buf);
}

int main(void) {
    puts("Testing loops and calculations...\n");
    
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;
        puts("i = ");
        print_number(i);
        puts(", sum = ");
        print_number(sum);
        puts("\n");
    }
    
    puts("Final sum: ");
    print_number(sum);
    puts("\n");
    
    puts("Loop test passed!\n");
    return 0;
}
