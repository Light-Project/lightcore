/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_DEVRES_H_
#define _DEVICE_DEVRES_H_

#include <irq.h>
#include <mm/gfp.h>
#include <resource.h>

struct devres;
typedef void (*devres_release_t)(struct device *dev, struct devres *res);
typedef bool (*devres_find_t)(struct device *dev, struct devres *res, const void *data);

struct devres {
    const char *name;
    struct list_head list;
    devres_release_t release;
};

/* device resource manager */
extern struct devres *devres_find(struct device *dev, devres_find_t find, const void *data);
extern void devres_insert(struct device *dev, struct devres *res);
extern void devres_remove(struct device *dev, struct devres *res);
extern void devres_release(struct device *dev, struct devres *res);
extern struct devres *devres_find_remove(struct device *dev, devres_find_t find, const void *data);
extern struct devres *devres_find_release(struct device *dev, devres_find_t find, const void *data);
extern void devres_release_all(struct device *dev);
extern void __init devres_init(void);

/* device resource alloc function */
extern void __malloc *dev_kmalloc(struct device *dev, size_t size, gfp_t flags);
extern void __malloc *dev_kzalloc(struct device *dev, size_t size, gfp_t flags);
extern void __malloc *dev_kmalloc_align(struct device *dev, size_t size, gfp_t flags, size_t align);
extern void __malloc *dev_kzalloc_align(struct device *dev, size_t size, gfp_t flags, size_t align);
extern void __malloc *dev_vmalloc(struct device *dev, size_t size);
extern void __malloc *dev_ioremap_node(struct device *dev, phys_addr_t addr, size_t size, gvm_t flags);
extern void __malloc *dev_ioremap_wc(struct device *dev, phys_addr_t addr, size_t size);
extern void __malloc *dev_ioremap_wt(struct device *dev, phys_addr_t addr, size_t size);
extern void __malloc *dev_ioremap(struct device *dev, phys_addr_t addr, size_t size);
extern void __malloc *dev_ioremap_resource(struct device *dev, struct resource *res);
extern state dev_irq_request(struct device *dev, irqnr_t vector, enum irq_flags flags, irq_handler_t handler, void *data, const char *name);

/* device resource release function */
extern void dev_kfree(struct device *dev, void *block);
extern void dev_vfree(struct device *dev, void *block);
extern void dev_iounmap(struct device *dev, void *block);
extern void dev_irq_release(struct device *dev, irqnr_t vector);

#define BUS_DEVRES_GENERIC(busn, type, member)                                  \
static __always_inline void __malloc *                                          \
busn##_kmalloc(type *bdev, size_t size, gfp_t flags)                            \
{                                                                               \
    return dev_kmalloc(&bdev->member, size, flags);                             \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_kzalloc(type *bdev, size_t size, gfp_t flags)                            \
{                                                                               \
    return dev_kzalloc(&bdev->member, size, flags);                             \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_kmalloc_node(type *bdev, size_t size, gfp_t flags, size_t align)         \
{                                                                               \
    return dev_kmalloc_align(&bdev->member, size, flags, align);                \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_kzalloc_align(type *bdev, size_t size, gfp_t flags, size_t align)        \
{                                                                               \
    return dev_kzalloc_align(&bdev->member, size, flags, align);                \
}                                                                               \
static __always_inline void                                                     \
busn##_kfree(type *bdev, void *block)                                           \
{                                                                               \
    return dev_kfree(&bdev->member, block);                                     \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_ioremap_wc(type *bdev, phys_addr_t addr, size_t size)                    \
{                                                                               \
    return dev_ioremap_wc(&bdev->member, addr, size);                           \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_ioremap_wt(type *bdev, phys_addr_t addr, size_t size)                    \
{                                                                               \
    return dev_ioremap_wt(&bdev->member, addr, size);                           \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_ioremap(type *bdev, phys_addr_t addr, size_t size)                       \
{                                                                               \
    return dev_ioremap(&bdev->member, addr, size);                              \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_ioremap_resource(type *bdev, struct resource *res)                       \
{                                                                               \
    return dev_ioremap_resource(&bdev->member, res);                            \
}                                                                               \
static __always_inline void                                                     \
busn##_iounmap(type *bdev, void *block)                                         \
{                                                                               \
    return dev_iounmap(&bdev->member, block);                                   \
}                                                                               \
static __always_inline void __malloc *                                          \
busn##_vmalloc(type *bdev, size_t size)                                         \
{                                                                               \
    return dev_vmalloc(&bdev->member, size);                                    \
}                                                                               \
static __always_inline void                                                     \
busn##_vfree(type *bdev, void *block)                                           \
{                                                                               \
    return dev_kfree(&bdev->member, block);                                     \
}                                                                               \
static __always_inline state                                                    \
busn##_irq_request(type *bdev, irqnr_t vector, enum irq_flags flags,            \
                   irq_handler_t handler, void *data, const char *name)         \
{                                                                               \
    return dev_irq_request(&bdev->member, vector, flags, handler, data, name);  \
}                                                                               \
static __always_inline void                                                     \
busn##_irq_release(type *bdev, irqnr_t vector)                                  \
{                                                                               \
    return dev_irq_release(&bdev->member, vector);                              \
}

#endif  /* _DEVICE_DEVRES_H_ */
