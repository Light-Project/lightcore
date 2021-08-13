#include <kboot.h>
#include <size.h>

void main(void)
{
    state ret;
    
    /* clean bss */
    memset(bss_start, 0, bss_size);

    console_clear();

    /* Init console */
    pr_init(console_print);

    /* Setup segment */
    segment_init();

    /* Initialization heap */
    heap_init(heap_start, heap_size);

    /* Initialization MMU */
    kernel_map();

    ret = memtest(kernel_entry, size_1MiB * 48);
    if (ret)
         panic("The kernel needs at least 64M "
               "of complete memory\n");

    extract_kernel(kernel_entry, piggy_start, piggy_size);

    pr_boot("Boot to kernel...");
    /* jmp to kernel */
    kernel_start(kernel_entry);
}
