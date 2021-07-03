#include <kboot.h>
#include <size.h>

void main(void)
{
    /* clean bss */
    memset(bss_start, 0, bss_size);
    
    video_clear();
    
    /* Init console */
    pr_init(video_print);
    
    /* Setup segment */
    segment_init();
    
    /* Initialization heap */
    heap_init(heap_start, heap_size);

    /* Initialization MMU */
    kernel_map();
    
    extract_kernel(kernel_entry, piggy_start, piggy_size);
    
    pr_boot("Boot to kernel...");
    /* jmp to kernel */
    kernel_start(kernel_entry);
}
