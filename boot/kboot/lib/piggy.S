/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <linkage.h>
    .section    .piggydata, "a"
piggy_start:
    .incbin     "boot/piggy_data"
