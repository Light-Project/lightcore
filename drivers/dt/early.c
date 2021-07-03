#include <state.h> 
#include <stddef.h>
#include <init/init.h>
#include <printk.h>
#include <crc/crc.h>
#include <driver/dt.h>
#include <driver/dt/libfdt.h>

#include "dt_private.h"

bool __init early_init_dt_scan_chosen(unsigned long node, const char *uname, int depth, void *data)
{
    int len;
    const char *args;
    const void *rng_seed;

    /* Matching nodes */
    if (depth != 1 || !data ||
        (strcmp(uname, "chosen") != 0 && strcmp(uname, "chosen@0") != 0))
        return 0;

    /* Retrieve command line */
    args = dt_get_prop(node, "bootargs", &len);
    if (args != NULL && len > 0)
        strlcpy(data, args, min(len, boot_args_size));

    rng_seed = dt_get_prop(node, "rng-seed", &len);
    if (rng_seed && len > 0) {
        // add_bootloader_randomness(rng_seed, l);

        /* try to clear seed so it won't be found. */
        fdt_nop_property(dt_start_addr, node, "rng-seed");

        /* update CRC check value */
        dt_crc32 = crc32(dt_start_addr, fdt_totalsize(dt_start_addr), ~0);
    }

    return 1;
}

bool __init early_dt_scan_node()
{
    bool ret;
    /* Retrieve various information from the /chosen node */
    ret = dt_scan_node(early_init_dt_scan_chosen, boot_args);
    if (ret)
        pr_warn("No chosen node found, continuing without\n");

    return ret;   
}

bool __init early_dt_verify(void *dt_start)
{
    if (!dt_start)
        return false;

    /* check device tree validity */
    if (fdt_check_header(dt_start))
        return false;

    /* Setup flat device-tree pointer */
    dt_start_addr = dt_start;
    dt_crc32 = crc32(dt_start_addr, fdt_totalsize(dt_start_addr), ~0);
    return true;
}

bool __init early_dt_scan(void *dt_start)
{
    bool status;
    
    status = early_dt_verify(dt_start);
    if(status == false)
        return false;
    
    early_dt_scan_node();
    return true;
}
