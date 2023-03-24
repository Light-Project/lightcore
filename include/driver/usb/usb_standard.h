/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_USB_STANDARD_H_
#define _DRIVER_USB_STANDARD_H_

#include <types.h>

struct usb_ctrlrequest {
    uint8_t bRequestType;
    uint8_t bRequest;

    le16    wValue;
    le16    wIndex;
    le16    wLength;
} __packed;

enum bRequestType {
    USB_DIR_OUT                 = 0x00,
    USB_DIR_IN                  = 0x80,
};

enum bRequest {
    USB_REQ_GET_STATUS          = 0x00,
    USB_REQ_CLEAR_FEATURE       = 0x01,
    USB_REQ_SET_FEATURE         = 0x03,
    USB_REQ_SET_ADDRESS         = 0x05,
    USB_REQ_GET_DESCRIPTOR      = 0x06,
    USB_REQ_SET_DESCRIPTOR      = 0x07,
    USB_REQ_GET_CONFIGURATION   = 0x08,
    USB_REQ_SET_CONFIGURATION   = 0x09,
    USB_REQ_GET_INTERFACE       = 0x0a,
    USB_REQ_SET_INTERFACE       = 0x0b,
    USB_REQ_SYNCH_FRAME         = 0x0c,
    USB_REQ_SET_ENCRYPTION      = 0x0d,
    USB_REQ_GET_ENCRYPTION      = 0x0e,
    USB_REQ_RPIPE_ABORT         = 0x0e,
    USB_REQ_SET_HANDSHAKE       = 0x0f,
    USB_REQ_RPIPE_RESET         = 0x0f,
    USB_REQ_GET_HANDSHAKE       = 0x10,
    USB_REQ_SET_CONNECTION      = 0x11,
    USB_REQ_SET_SECURITY_DATA   = 0x12,
    USB_REQ_GET_SECURITY_DATA   = 0x13,
    USB_REQ_SET_WUSB_DATA       = 0x14,
    USB_REQ_LOOPBACK_DATA_WRITE = 0x15,
    USB_REQ_LOOPBACK_DATA_READ  = 0x16,
    USB_REQ_SET_INTERFACE_DS    = 0x17,

    USB_REQ_SET_SEL             = 0x30,
    USB_REQ_SET_ISOCH_DELAY     = 0x31,
};

enum usb_req_status {
    USB_DEV_STAT_U1_ENABLED         = 2,    /* transition into U1 state */
    USB_DEV_STAT_U2_ENABLED         = 3,    /* transition into U2 state */
    USB_DEV_STAT_LTM_ENABLED        = 4,    /* Latency tolerance messages */
};

enum usb_req_feature {
    USB_DEVICE_SELF_POWERED         = 0,    /* (read only) */
    USB_DEVICE_REMOTE_WAKEUP        = 1,    /* dev may initiate wakeup */
    USB_DEVICE_TEST_MODE            = 2,    /* (wired high speed only) */
    USB_DEVICE_BATTERY              = 2,    /* (wireless) */
    USB_DEVICE_B_HNP_ENABLE         = 3,    /* (otg) dev may initiate HNP */
    USB_DEVICE_WUSB_DEVICE          = 3,    /* (wireless)*/
    USB_DEVICE_A_HNP_SUPPORT        = 4,    /* (otg) RH port supports HNP */
    USB_DEVICE_A_ALT_HNP_SUPPORT    = 5,    /* (otg) other RH port does */
    USB_DEVICE_DEBUG_MODE           = 6,    /* (special devices only) */
};

struct usb_descriptor_header {
    uint8_t bLength;
    uint8_t bDescriptorType;
} __packed;

enum bDescriptorType {
    USB_DT_DEVICE                   = 0x01,
    USB_DT_CONFIG                   = 0x02,
    USB_DT_STRING                   = 0x03,
    USB_DT_INTERFACE                = 0x04,
    USB_DT_ENDPOINT                 = 0x05,
    USB_DT_DEVICE_QUALIFIER         = 0x06,
    USB_DT_OTHER_SPEED_CONFIG       = 0x07,
    USB_DT_INTERFACE_POWER          = 0x08,
    USB_DT_OTG                      = 0x09,
    USB_DT_DEBUG                    = 0x0a,
    USB_DT_INTERFACE_ASSOCIATION    = 0x0b,
    USB_DT_SECURITY                 = 0x0c,
    USB_DT_KEY                      = 0x0d,
    USB_DT_ENCRYPTION_TYPE          = 0x0e,
    USB_DT_BOS                      = 0x0f,
    USB_DT_DEVICE_CAPABILITY        = 0x10,
    USB_DT_WIRELESS_ENDPOINT_COMP   = 0x11,
    USB_DT_WIRE_ADAPTER             = 0x21,
    USB_DT_RPIPE                    = 0x22,
    USB_DT_CS_RADIO_CONTROL         = 0x23,
    USB_DT_PIPE_USAGE               = 0x24,
    USB_DT_SS_ENDPOINT_COMP         = 0x30,
    USB_DT_SSP_ISOC_ENDPOINT_COMP   = 0x31,
};

struct usb_hub_descriptor {
    uint8_t bDescLength;
    uint8_t bDescriptorType;

    uint8_t bNbrPorts;
    le16    wHubCharacteristics;
    uint8_t bPwrOn2PwrGood;
    uint8_t bHubContrCurrent;

    union {
        struct {
        };
        struct {
            uint8_t bHubHdrDecLat;
            le16 wHubDelay;
            le16 DeviceRemovable;
        };
    };
} __packed;


#define HUB_LED_AUTO	0
#define HUB_LED_AMBER	1
#define HUB_LED_GREEN	2
#define HUB_LED_OFF	3

struct usb_device_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;

    le16    bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    le16    idVendor;
    le16    idProduct;
    le16    bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __packed;

enum bDeviceClass {
    USB_CLASS_PER_INTERFACE         = 0x00,
    USB_CLASS_AUDIO                 = 0x01,
    USB_CLASS_COMM                  = 0x02,
    USB_CLASS_HID                   = 0x03,
    USB_CLASS_PHYSICAL              = 0x05,
    USB_CLASS_STILL_IMAGE           = 0x06,
    USB_CLASS_PRINTER               = 0x07,
    USB_CLASS_MASS_STORAGE          = 0x08,
    USB_CLASS_HUB                   = 0x09,
    USB_CLASS_CDC_DATA              = 0x0a,
    USB_CLASS_CSCID                 = 0x0b,
    USB_CLASS_CONTENT_SEC           = 0x0d,
    USB_CLASS_VIDEO                 = 0x0e,
    USB_CLASS_WIRELESS_CONTROLLER   = 0xe0,
    USB_CLASS_PERSONAL_HEALTHCARE   = 0x0f,
    USB_CLASS_AUDIO_VIDEO           = 0x10,
    USB_CLASS_BILLBOARD             = 0x11,
    USB_CLASS_USB_TYPE_C_BRIDGE     = 0x12,
    USB_CLASS_MISC                  = 0xef,
    USB_CLASS_APP_SPEC              = 0xfe,
    USB_CLASS_VENDOR_SPEC           = 0xff,
};

struct usb_config_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;

    le16    wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} __packed;

enum config_bmAttributes {
    USB_CONFIG_ATT_ONE          = 1 << 7,
    USB_CONFIG_ATT_SELFPOWER    = 1 << 6,
    USB_CONFIG_ATT_WAKEUP       = 1 << 5,
    USB_CONFIG_ATT_BATTERY      = 1 << 4,
};

struct usb_endpoint_descriptor {
    uint8_t bRequestType;
    uint8_t bRequest;

    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    le16    wMaxPacketSize;
    uint8_t bInterval;

    uint8_t bRefresh;
    uint8_t bSynchAddress;
} __packed;

enum endpoint_bmAttributes {
    USB_ENDPOINT_XFER_CONTROL   = 0,
    USB_ENDPOINT_XFER_ISOC      = 1,
    USB_ENDPOINT_XFER_BULK      = 2,
    USB_ENDPOINT_XFER_INT       = 3,
    USB_ENDPOINT_XFER_MASK      = 3,
};

#endif /* _DRIVER_USB_STANDARD_H_ */
