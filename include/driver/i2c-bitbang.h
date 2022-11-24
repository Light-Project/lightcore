/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_BITBANG_H_
#define _DRIVER_I2C_BITBANG_H_

#include <driver/i2c.h>

struct i2c_bitbang {
	bool (*getsda)(void *data);
	bool (*getscl)(void *data);
	void (*setsda)(void *data, bool value);
	void (*setscl)(void *data, bool value);

	state (*prepare)(void *data);
	void (*finish)(void *data);

    unsigned int delay;
    unsigned int timeout;
    void *data;
};

extern state i2c_bitbang_register(struct i2c_host *host);

#endif  /* _DRIVER_I2C_BITBANG_H_ */
