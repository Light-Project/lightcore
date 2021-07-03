/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IOPORT_H_ 
#define _IOPORT_H_

#include <types.h>
#include <state.h>

state ioport_alloc(size_t start, size_t size);

#endif /* _IOPORT_H_ */
