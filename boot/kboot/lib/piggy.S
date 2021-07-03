/* SPDX-License-Identifier: GPL-2.0 */

#include <linkage.h>
    .section    .piggydata, "a"
piggy_start:
    .incbin     "boot/piggy_data"
