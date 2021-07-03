#include <driver/platform.h>
#include <driver/dt.h> 
#include <init/initcall.h>

const struct dt_device_id dt_bus_match_table[] = {
    { .compatible = "simple-bus", },
    { .compatible = "simple-mfd", },
    { .compatible = "isa", },
	{} /* NULL */
};


/**
 * platform_dt_bus_probe - Create platform devices for all devices of dt bus
 * 
 */
state platform_dt_bus_probe(struct dt_node *bus)
{
	struct dt_node *child;

    dt_for_each_child_node(bus, child) {

    }

    return -ENOERR;
}

/**
 * platform_dt_populate - Populate device tree to platform device
 * 
 */
state platform_dt_populate(struct dt_node *root, 
                           const struct dt_device_id *matches)
{
    // struct dt_node *node;


    return -ENOERR;
}

state __init platform_dt_init(void)
{


    return -ENOERR;
}

late_initcall_sync(platform_dt_init);
