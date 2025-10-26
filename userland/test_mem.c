// 内存分配测试程序

extern void puts(const char* str);
extern void* sbrk(int increment);

int main(void) {
    puts("Testing memory allocation (sbrk)...\n");
    
    // 分配 1KB 内存
    char* buf = (char*)sbrk(1024);
    if (buf == (char*)-1) {
        puts("ERROR: sbrk failed\n");
        return 1;
    }
    
    puts("Allocated 1KB memory\n");
    
    // 写入数据
    for (int i = 0; i < 26; i++) {
        buf[i] = 'A' + i;
    }
    buf[26] = '\0';
    
    puts("Written data: ");
    puts(buf);
    puts("\n");
    
    puts("Memory test passed!\n");
    return 0;
}
