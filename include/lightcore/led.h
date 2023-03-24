/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_LED_H_
#define _LIGHTCORE_LED_H_

enum led_brightness {
    LED_BRIGHTNESS_OFF      = 0,
    LED_BRIGHTNESS_ON       = 1,
    LED_BRIGHTNESS_DARK     = 63,
    LED_BRIGHTNESS_AVERAGE  = 127,
    LED_BRIGHTNESS_FULL     = 255,
};

#endif /* _LIGHTCORE_LED_H_ */
