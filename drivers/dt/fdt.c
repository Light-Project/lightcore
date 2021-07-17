#include <driver/dt.h>
#include <driver/dt/libfdt.h>
#include "dt_private.h"

uint32_t dt_crc32;
void *dt_start_addr = NULL;

state __init dt_scan_node(state (*fn)(unsigned long node,
        const char *uname, int depth, void *data), void *data)
{
    const void *node = dt_start_addr;
    const char *uname;
    int offset, rc = -1, depth = -1;

    if (!node)
        return -EINVAL;

    for (offset = fdt_next_node(node, -1, &depth);
         offset >= 0 && depth >= 0 && rc;
         offset = fdt_next_node(node, offset, &depth)) {
        uname = fdt_get_name(node, offset, NULL);
        rc = fn(offset, uname, depth, data);
    }

    return rc;
}

/**
 * of_scan_flat_dt_subnodes - scan sub-nodes of a node call callback on each.
 * @it: callback function
 * @data: context data pointer
 *
 * This function is used to scan sub-nodes of a node.
 */
int __init dt_scan_subnode(unsigned long parent,
				    int (*it)(unsigned long node,
					      const char *uname,
					      void *data),
				    void *data)
{
	const void *blob = dt_start_addr;
	int node;

	fdt_for_each_subnode(node, blob, parent) {
		const char *pathp;
		int rc;

		pathp = fdt_get_name(blob, node, NULL);
		rc = it(node, pathp, data);
		if (rc)
			return rc;
	}
	return 0;
}

/**
 * of_get_flat_dt_subnode_by_name - get the subnode by given name
 *
 * @node: the parent node
 * @uname: the name of subnode
 * @return offset of the subnode, or -FDT_ERR_NOTFOUND if there is none
 */

int __init dt_get_subnode_by_name(unsigned long node, const char *uname)
{
	return fdt_subnode_offset(dt_start_addr, node, uname);
}

/**
 * of_get_flat_dt_root - find the root node in the flat blob
 */
unsigned long __init dt_get_root(void)
{
	return 0;
}

/**
 * of_get_flat_dt_prop - Given a node in the flat blob, return the property ptr
 *
 * This function can be used within scan_flattened_dt callback to get
 * access to properties
 */
const void *__init dt_get_prop(unsigned long node, const char *name,
				       int *size)
{
	return fdt_getprop(dt_start_addr, node, name, size);
}
