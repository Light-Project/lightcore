/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _POWER_H_
#define _POWER_H_

/**
 * @uninit:
 * @cold: use power device for full power mode switching
 * @warm: use power device for trigger restart line
 * @soft:
 * @hard:
 */
enum restart_mode {
    POWER_RESTART_UNINIT,
    POWER_RESTART_COLD,
    POWER_RESTART_WARM,
    POWER_RESTART_SOFT,
    POWER_RESTART_HARD,
};

extern __noreturn void kernel_restart(enum restart_mode mode);
extern __noreturn void kernel_reboot();
extern __noreturn void kernel_poweroff(void);
extern __noreturn void kernel_halt(void);

#endif  /* _POWER_H_ */
