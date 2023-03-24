/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_RAW_H_
#define _DEVICE_RAW_H_

struct raw_channel {
    unsigned int channel;
};

struct raw_device {
    struct raw_channel *channels;
    unsigned int channels_nr;
    struct raw_ops *ops;
};

struct raw_ops {
    state (*read)(struct raw_channel *, ssize_t *val);
    state (*write)(struct raw_channel *, ssize_t val);
};

#endif /* _DEVICE_RAW_H_ */
