#ifndef _LIGHTCORE_VIDEO_H_
#define _LIGHTCORE_VIDEO_H_

#include <lightcore/types.h>

struct video_image {
    uint32_t dx;
    uint32_t dy;
    uint32_t width;
    uint32_t height;
    uint8_t depth;
	const char *image;
};

struct video_info {


};

#endif /* _LIGHTCORE_VIDEO_H_ */
