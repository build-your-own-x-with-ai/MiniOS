/* Mini OS 链接脚本 */

ENTRY(_start)

SECTIONS
{
    /* 内核加载到 1MB 地址 */
    . = 0x00100000;

    .text ALIGN(4K) : {
        *(.multiboot)
        *(.text)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }

    /* 内核结束标记 */
    _kernel_end = .;
}
