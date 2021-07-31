/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_SOUND_H_
#define _DEVICE_SOUND_H_

#include <device.h>

struct snd_pcm_hardware {

};

struct snd_pcm_stream {

};

struct snd_pcm_ops {
    state (*open)(struct snd_pcm_stream *);
    state (*close)(struct snd_pcm_stream *);
    state (*trigger)(struct snd_pcm_stream *);
};

struct snd_device {
    const char *name;
    struct list_head list;

};

#endif  /* _DEVICE_SOUND_H_ */
