/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PWM_H_
#define _DRIVER_PWM_H_

enum pwm_polarity {
	PWM_POLARITY_NORMAL,
	PWM_POLARITY_REVERSAL,
};

struct pwm_state {
    unsigned int period;
    unsigned int duty;
    enum pwm_polarity polarity;
};

struct pwm_channel {
    struct pwm_device *pwm;
    struct list_head list;
    unsigned int index;
};

struct pwm_device {
    struct list_head channel;
    struct pwm_ops *ops;
};

struct pwm_ops {
    state (*enable)(struct pwm_device *, unsigned int index);
    void (*disable)(struct pwm_device *, unsigned int index);
    state (*get_state)(struct pwm_device *, unsigned int index, struct pwm_state *);
    state (*set_state)(struct pwm_device *, unsigned int index, struct pwm_state *);
};

state pwm_register(struct pwm_device *pwm);
void pwm_unregister(struct pwm_device *pwm);

#endif /* _DRIVER_PWM_H_ */
