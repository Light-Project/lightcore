/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <mm.h>
#include <mm.h>
#include <driver/usb.h>
#include <lightcore/asm/byteorder.h>

static int usb_request_start(struct usb_request *request, int timeout)
{
    int retval;

    retval = usb_request_submit(request);

    return retval;
}

/**
 * usb_control_transfer - initiate a USB control transfer and wait for the transfer to end
 * @udev: usb device for control transmission
 * @pipe: endpoint "pipe" for send the message
 * @bRequestType: usb transfer request type value
 * @bRequest: usb transfer request value
 * @wValue: usb transfer value
 * @wIndex: usb transfer index value
 * @data: memory pointer to exchange data
 * @wLength: length of transfer to send
 * @timeout: time in msecs to wait for message to return
 *
 * @return: The length is returned if the transmission succeeds,
 * and the error value is returned if the transmission fails
 */
int usb_control_transfer(struct usb_device *udev, unsigned int pipe, uint8_t bRequestType,
                         uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                         void *data, uint16_t wLength, int timeout)
{
    struct usb_ctrlrequest *ctrlrequest;
    struct usb_request *request;
    unsigned int ret;

    ctrlrequest = kzalloc(sizeof(*ctrlrequest) + sizeof(*request), GFP_KERNEL);
    request = (void *)((char *)ctrlrequest + sizeof(*ctrlrequest));
    if (!ctrlrequest)
        return -ENOMEM;

    ctrlrequest->bRequestType = bRequestType;
    ctrlrequest->bRequest = bRequest;
    ctrlrequest->wValue = cpu_to_le16(wValue);
    ctrlrequest->wIndex = cpu_to_le16(wIndex);
    ctrlrequest->wLength = cpu_to_le16(wLength);

    request->device = udev;
    request->pipe = pipe;
    request->ctrl = ctrlrequest;
    request->transfer_buff = data;
    request->transfer_len = wLength;
    request->pipe = pipe;

    ret = usb_request_start(request, timeout);

    kfree(ctrlrequest);

    return ret;
}

int usb_control_transfer_recv(struct usb_device *udev, unsigned int pipe, uint8_t bRequestType,
                              uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                              void **data, uint16_t wLength, int timeout, gfp_t gfp)
{
    int ret;

    *data = kmalloc(wLength, gfp);
    if (!*data)
        return -ENOMEM;

    ret = usb_control_transfer(udev, pipe, bRequestType, bRequest,
                               wValue, wIndex, *data, wLength, timeout);
    if (ret < 0)
        kfree(*data);

    return ret;
}

int usb_control_transfer_send(struct usb_device *udev, unsigned int pipe, uint8_t bRequestType,
                              uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                              void *data, uint16_t wLength, int timeout, gfp_t gfp)
{
    void *buff;
    int ret;

    buff = kmalloc(wLength, gfp);
    if (!buff)
        return -ENOMEM;

    memcpy(buff, data, wLength);

    ret = usb_control_transfer(udev, pipe, bRequestType, bRequest,
                         wValue, wIndex, buff, wLength, timeout);

    kfree(buff);

    return ret < 0 ? ret : 0;
}

/**
 * usb_bulk_transfer - initiate a USB bulk transfer and wait for the transfer to end
 * @udev: usb device for control transmission
 * @pipe: endpoint "pipe" for send the message
 * @wLength: length of transfer to send
 * @timeout: time in msecs to wait for message to return
 *
 * @return: The length is returned if the transmission succeeds,
 * and the error value is returned if the transmission fails
 */
int usb_bulk_transfer(struct usb_device *udev, unsigned int pipe,
                      void *data, int length, int *transferred, int timeout)
{
    struct usb_request *request;
    struct usb_endpoint *ep;

    ep = usb_pipe_endpoint(udev, pipe);
    if (!ep)
        return -EINVAL;

    request = kzalloc(sizeof(*request), GFP_KERNEL);
    if (!request)
        return -ENOMEM;

    if (ep->descriptor.bmAttributes & USB_ENDPOINT_XFER_MASK) {

    } else {

    }

    request->device = udev;
    request->pipe = pipe;
    request->transfer_buff = data;
    request->transfer_len = length;
    request->pipe = pipe;

    return usb_request_start(request, timeout);
}

/**
 * usb_interrupt_transfer - initiate a USB interrupt transfer and wait for the transfer to end
 * @udev: usb device for control transmission
 * @pipe: endpoint "pipe" for send the message
 * @wLength: length of transfer to send
 * @timeout: time in msecs to wait for message to return
 *
 * @return: The length is returned if the transmission succeeds,
 * and the error value is returned if the transmission fails
 */
int usb_interrupt_transfer(struct usb_device *udev, unsigned int pipe,
                          void *data, int len, int *transferred, int timeout)
{
    return usb_bulk_transfer(udev, pipe, data, len, transferred, timeout);
}


state usb_descriptor_read(struct usb_device *udev, uint8_t type,
                          uint8_t index, void *buff, unsigned int size)
{
    int retval, retry = 3;

    memset(buff, 0, size);

    for (retry = 0; retry < 3; ++retry) {
        retval = usb_control_transfer(udev, usb_pipe_ctlrcv(udev, 0),
                    USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, (type << 8) + index,
                    0, buff, size, USB_CTRL_TIMEOUT);
        if (retval <= 0)
            continue;
        if (retval > 1 && ((uint8_t *)buff)[1] != type) {
            retval = -ENODATA;
            continue;
        }
        break;
    }

    return retval;
}

/**
 * usb_descriptor_get - device descriptor settings
 * @dev: device to read device descriptor
 * @size: how much of the descriptor to read
 *
 */
state usb_descriptor_get(struct usb_device *udev, unsigned int size)
{
	struct usb_device_descriptor *desc;

    if (size == 0)
        size = sizeof(*desc);

    if (size > sizeof(*desc))
        return -EINVAL;

    desc = &udev->descriptor;
    return usb_descriptor_read(udev, USB_DT_DEVICE, 0, desc, size);
}
