/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_BUZZER_H_
#define _LIGHTCORE_BUZZER_H_

#include <lightcore/ioctl.h>

enum buzzer_frequency {
    BUZZER_FREQ_C0      = 16,
    BUZZER_FREQ_CS0     = 17,
    BUZZER_FREQ_D0      = 18,
    BUZZER_FREQ_DS0     = 20,
    BUZZER_FREQ_E0      = 21,
    BUZZER_FREQ_F0      = 22,
    BUZZER_FREQ_FS0     = 23,
    BUZZER_FREQ_G0      = 25,
    BUZZER_FREQ_GS0     = 26,
    BUZZER_FREQ_A0      = 28,
    BUZZER_FREQ_AS0     = 29,
    BUZZER_FREQ_B0      = 31,

    BUZZER_FREQ_C1      = 33,
    BUZZER_FREQ_CS1     = 35,
    BUZZER_FREQ_D1      = 37,
    BUZZER_FREQ_DS1     = 39,
    BUZZER_FREQ_E1      = 41,
    BUZZER_FREQ_F1      = 44,
    BUZZER_FREQ_FS1     = 46,
    BUZZER_FREQ_G1      = 49,
    BUZZER_FREQ_GS1     = 52,
    BUZZER_FREQ_A1      = 55,
    BUZZER_FREQ_AS1     = 58,
    BUZZER_FREQ_B1      = 62,

    BUZZER_FREQ_C2      = 65,
    BUZZER_FREQ_CS2     = 69,
    BUZZER_FREQ_D2      = 73,
    BUZZER_FREQ_DS2     = 78,
    BUZZER_FREQ_E2      = 82,
    BUZZER_FREQ_F2      = 87,
    BUZZER_FREQ_FS2     = 93,
    BUZZER_FREQ_G2      = 98,
    BUZZER_FREQ_GS2     = 104,
    BUZZER_FREQ_A2      = 110,
    BUZZER_FREQ_AS2     = 117,
    BUZZER_FREQ_B2      = 123,

    BUZZER_FREQ_C3      = 131,
    BUZZER_FREQ_CS3     = 139,
    BUZZER_FREQ_D3      = 147,
    BUZZER_FREQ_DS3     = 156,
    BUZZER_FREQ_E3      = 165,
    BUZZER_FREQ_F3      = 175,
    BUZZER_FREQ_FS3     = 185,
    BUZZER_FREQ_G3      = 196,
    BUZZER_FREQ_GS3     = 208,
    BUZZER_FREQ_A3      = 220,
    BUZZER_FREQ_AS3     = 233,
    BUZZER_FREQ_B3      = 247,

    BUZZER_FREQ_C4      = 262,
    BUZZER_FREQ_CS4     = 277,
    BUZZER_FREQ_D4      = 294,
    BUZZER_FREQ_DS4     = 311,
    BUZZER_FREQ_E4      = 330,
    BUZZER_FREQ_F4      = 349,
    BUZZER_FREQ_FS4     = 370,
    BUZZER_FREQ_G4      = 392,
    BUZZER_FREQ_GS4     = 415,
    BUZZER_FREQ_A4      = 440,
    BUZZER_FREQ_AS4     = 466,
    BUZZER_FREQ_B4      = 494,

    BUZZER_FREQ_C5      = 523,
    BUZZER_FREQ_CS5     = 554,
    BUZZER_FREQ_D5      = 587,
    BUZZER_FREQ_DS5     = 622,
    BUZZER_FREQ_E5      = 659,
    BUZZER_FREQ_F5      = 698,
    BUZZER_FREQ_FS5     = 740,
    BUZZER_FREQ_G5      = 784,
    BUZZER_FREQ_GS5     = 831,
    BUZZER_FREQ_A5      = 880,
    BUZZER_FREQ_AS5     = 932,
    BUZZER_FREQ_B5      = 988,

    BUZZER_FREQ_C6      = 1047,
    BUZZER_FREQ_CS6     = 1109,
    BUZZER_FREQ_D6      = 1175,
    BUZZER_FREQ_DS6     = 1245,
    BUZZER_FREQ_E6      = 1319,
    BUZZER_FREQ_F6      = 1397,
    BUZZER_FREQ_FS6     = 1480,
    BUZZER_FREQ_G6      = 1568,
    BUZZER_FREQ_GS6     = 1661,
    BUZZER_FREQ_A6      = 1760,
    BUZZER_FREQ_AS6     = 1865,
    BUZZER_FREQ_B6      = 1976,

    BUZZER_FREQ_C7      = 2093,
    BUZZER_FREQ_CS7     = 2217,
    BUZZER_FREQ_D7      = 2349,
    BUZZER_FREQ_DS7     = 2489,
    BUZZER_FREQ_E7      = 2637,
    BUZZER_FREQ_F7      = 2794,
    BUZZER_FREQ_FS7     = 2960,
    BUZZER_FREQ_G7      = 3136,
    BUZZER_FREQ_GS7     = 3322,
    BUZZER_FREQ_A7      = 3520,
    BUZZER_FREQ_AS7     = 3729,
    BUZZER_FREQ_B7      = 3951,

    BUZZER_FREQ_C8      = 4186,
    BUZZER_FREQ_CS8     = 4435,
    BUZZER_FREQ_D8      = 4699,
    BUZZER_FREQ_DS8     = 4978,
};

enum buzzer_ioctl {
    BUZZER_IOCTL_ENABLE     = 0,
    BUZZER_IOCTL_DISABLE    = 1,
    BUZZER_IOCTL_FREQ_GET   = 2,
    BUZZER_IOCTL_FREQ_SET   = 3,
    BUZZER_IOCTL_PLAY       = 4,
};

struct buzzer_param {
    unsigned int length;
    struct {
        unsigned int freq;
        unsigned int delay;
    } table[0];
};

#define BUZZER_ENABLE       IOCTL_NONE('p', BUZZER_IOCTL_ENABLE)
#define BUZZER_DISABLE      IOCTL_NONE('p', BUZZER_IOCTL_DISABLE)
#define BUZZER_FREQ_GET     IOCTL_INPUT('p', BUZZER_IOCTL_FREQ_GET, sizeof(unsigned int *))
#define BUZZER_FREQ_SET     IOCTL_OUTPUT('p', BUZZER_IOCTL_FREQ_SET, sizeof(unsigned int))
#define BUZZER_PLAY         IOCTL_OUTPUT('p', BUZZER_IOCTL_PLAY, sizeof(struct buzzer_param))

#endif /* _LIGHTCORE_BUZZER_H_ */
