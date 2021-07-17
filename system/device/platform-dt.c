// #include <driver/platform.h>
// #include <driver/dt.h> 
// #include <init/initcall.h>

// const struct dt_device_id dt_bus_match_table[] = {
//     { .compatible = "simple-bus", },
//     { .compatible = "amba-bus", },
//     { .compatible = "isa-isa", },
// 	{ } /* NULL */
// };

// static bool pdt_device_match_one(struct dt_device_id *ids, struct platform_device*)
// {


//     return false;
// }

// static bool pdt_device_match(struct dt_device_id *ids, struct platform_device *pdev)
// {

//     return false;
// }

// static state pdt_add_device(struct dt_node *bus, struct device *parent, 
//                             struct dt_device_id *ids)
// {

//     return -ENOERR;
// }


// /**
//  * platform_dt_bus_probe - Create platform devices for all devices of dt bus
//  * 
//  */
// static state pdt_scan_bus(struct dt_node *bus)
// {
// 	struct dt_node *child;

//     dt_for_each_child_node(bus, child) {

//     }

//     return -ENOERR;
// }

// /**
//  * platform_dt_populate - Populate device tree to platform device
//  * 
//  */
// state platform_dt_populate(struct dt_node *root, 
//                            const struct dt_device_id *matches)
// {
//     // struct dt_node *node;


//     return -ENOERR;
// }

// state __init platform_dt_init(void)
// {


//     return -ENOERR;
// }

// late_initcall_sync(platform_dt_init);
