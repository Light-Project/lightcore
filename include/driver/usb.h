/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_USB_H_
#define _DRIVER_USB_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>
#include <resource.h>
#include <devtable.h>
#include <driver/usb/usb_standard.h>

extern struct bus_type usb_bus_type;

struct usb_ops;
struct usb_device;

enum usb_speed {
    USB_SPEED_UNKNOW,
    USB_SPEED_LOW,          /* USB 1.0 */
    USB_SPEED_FULL,         /* USB 1.1 */
    USB_SPEED_HIGH,         /* USB 2.0 */
    USB_SPEED_WIRELESS,     /* USB 2.5 */
    USB_SPEED_SUPER,        /* USB 3.0 */
    USB_SPEED_SUPER_PLUS,   /* USB 3.1 */
    USB_SPEED_SUPER_PLUS_2, /* USB 3.2 */
};

enum usb_host_type {
    USB_HOST_11,    /* USB host 1.1 */
    USB_HOST_2,     /* USB host 2.0 */
    USB_HOST_25,    /* USB host 2.5 */
    USB_HOST_3,     /* USB host 3.0 */
    USB_HOST_31,    /* USB host 3.1 */
    USB_HOST_32,    /* USB host 3.2 */
};

struct usb_request {
    struct list_head request_list;  /*  */

    struct usb_device *device;
    uint32_t pipe;

    struct usb_ctrlrequest *ctrl;   /* For control transmission only */
    void *transfer_buff;
    int   transfer_len;
};

struct usb_endpoint {
    struct usb_endpoint_descriptor descriptor;

    struct list_head request_list;
};

struct usb_device {
    struct usb_bus *bus;

    enum usb_speed speed;
    unsigned int devnr;

    struct device device;   /* Generic device */
    struct usb_device_descriptor descriptor;

    struct usb_endpoint *endpoint_in[16];
    struct usb_endpoint *endpoint_out[16];
};

#define device_to_usb_device(dpt) \
        (container_of((dpt), struct usb_device, device))

struct usb_driver {
    struct driver driver;
    struct list_head list;
    const struct usb_device_id *id_table;

    /* Operation function */
    state (*probe)(struct usb_device *pdev, const void *data);
    state (*remove)(struct usb_device *pdev);
    void (*shutdown)(struct usb_device *pdev);
    state (*suspend)(struct usb_device *pdev, pm_message_t state);
    state (*resume)(struct usb_device *pdev);
};

#define driver_to_usb_driver(dpt) \
    container_of((dpt), struct usb_driver, driver)

struct usb_bus {
    struct usb_device *root_hub;
};

struct usb_host {
    struct usb_bus bus;
    struct usb_ops *ops;
    struct device *dev;
    enum usb_host_type type;
};

#define usb_bus_to_host(bpt) \
    container_of((bpt), struct usb_host, bus)

struct usb_ops {
    state (*setup)(struct usb_host *);
    state (*start)(struct usb_host *);
    state (*stop)(struct usb_host *);

    state (*enqueue)(struct usb_host *, struct usb_request *);
    state (*dequeue)(struct usb_host *, struct usb_request *);

    void (*ep_disable)(struct usb_host *, struct usb_endpoint *);
};

#define USB_CTRL_TIMEOUT	5000

#define USB_PIPE_DEVICE(dev)    ((dev->devnr) & 0xff)
#define USB_PIPE_ENDPOINT(ep)   (((ep) & 0xff) << 8)
#define USB_PIPE_ISOCHRONOUS    (0 << 24)
#define USB_PIPE_INTERRUPT      (1 << 24)
#define USB_PIPE_CONTROL        (2 << 24)
#define USB_PIPE_BULK           (3 << 24)
#define USB_PIPE_IN             (0 << 28)
#define USB_PIPE_OUT            (1 << 28)

#define usb_pipe_ctlrcv(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_CONTROL | USB_PIPE_IN)
#define usb_pipe_ctlsnd(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_CONTROL | USB_PIPE_OUT)
#define usb_pipe_isocrcv(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_ISOCHRONOUS | USB_PIPE_IN)
#define usb_pipe_isocsnd(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_ISOCHRONOUS | USB_PIPE_OUT)
#define usb_pipe_bulkrcv(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_BULK | USB_PIPE_IN)
#define usb_pipe_bulksnd(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_BULK | USB_PIPE_OUT)
#define usb_pipe_intrcv(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_INTERRUPT | USB_PIPE_IN)
#define usb_pipe_intsnd(dev, endpoint) \
    (USB_PIPE_DEVICE(dev) | USB_PIPE_ENDPOINT(endpoint) | USB_PIPE_INTERRUPT | USB_PIPE_OUT)

#define usb_pipedevice(pipe)    ((pipe) & 0xff)
#define usb_pipeendpoint(pipe)  (((pipe) & 0xff) >> 8)

#define usb_pipetype(pipe)      ((pipe) & (3 << 24))
#define usb_pipectl(pipe)       (usb_pipetype(pipe) == USB_PIPE_CONTROL)
#define usb_pipeisoc(pipe)      (usb_pipetype(pipe) == USB_PIPE_ISOCHRONOUS)
#define usb_pipebulk(pipe)      (usb_pipetype(pipe) == USB_PIPE_BULK)
#define usb_pipeint(pipe)       (usb_pipetype(pipe) == USB_PIPE_INTERRUPT)

#define usb_pipein(pipe)        ((pipe) & USB_PIPE_OUT)
#define usb_pipeout(pipe)       (!usb_pipein(pipe))

static inline struct usb_endpoint *
usb_pipe_endpoint(struct usb_device *udev, unsigned int pipe)
{
    struct usb_endpoint **endpoint;
    endpoint = usb_pipein(pipe) ? udev->endpoint_in : udev->endpoint_out;
    return endpoint[usb_pipeendpoint(pipe)];
}

/* transfer.c */
int usb_control_transfer(struct usb_device *udev, unsigned int pipe, uint8_t bRequestType,
                         uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                         void *data, uint16_t wLength, int timeout);
int usb_bulk_transfer(struct usb_device *udev, unsigned int pipe,
                      void *data, int len, int *transferred, int timeout);
int usb_interrupt_transfer(struct usb_device *udev, unsigned int pipe,
                           void *data, int len, int *transferred, int timeout);
state usb_descriptor_read(struct usb_device *udev, uint8_t type,
                          uint8_t index, void *buff, unsigned int size);
state usb_descriptor_get(struct usb_device *udev, unsigned int size);

/* driver.c */
state usb_driver_register(struct usb_driver *);

/* request.c */
int usb_request_submit(struct usb_request *request);

/* core.c */
state usb_host_request_submit(struct usb_request *request);
state usb_host_register(struct usb_host *);

#endif  /* _DRIVER_USB_H_ */
