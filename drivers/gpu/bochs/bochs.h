#ifndef _DRIVER_GPU_BOCHS_H_
#define _DRIVER_GPU_BOCHS_H_

/* bochs_hw.c */
int bochs_hw_init(struct drm_device *dev);
void bochs_hw_fini(struct drm_device *dev);

void bochs_hw_setmode(struct bochs_device *bochs,
		      struct drm_display_mode *mode);
void bochs_hw_setformat(struct bochs_device *bochs,
			const struct drm_format_info *format);
void bochs_hw_setbase(struct bochs_device *bochs,
		      int x, int y, int stride, u64 addr);
int bochs_hw_load_edid(struct bochs_device *bochs);

/* bochs_mm.c */
int bochs_mm_init(struct bochs_device *bochs);
void bochs_mm_fini(struct bochs_device *bochs);

/* bochs_kms.c */
int bochs_kms_init(struct bochs_device *bochs);

/* bochs_fbdev.c */
extern const struct drm_mode_config_funcs bochs_mode_funcs;
 
