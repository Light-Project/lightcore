/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_FBCON_H_
#define _DRIVER_FBCON_H_

#include <types.h>
#include <state.h>
#include <driver/video.h>

state fbcon_register(struct video_device *vdev); 


#endif /* _DRIVER_FBCON_H_ */
