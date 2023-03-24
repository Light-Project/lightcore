/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DT_BINDINGS_IRQ_H_
#define _DT_BINDINGS_IRQ_H_

#define IRQ_TYPE_NONE           0
#define IRQ_TYPE_EDGE_RISING    1
#define IRQ_TYPE_EDGE_FALLING   2
#define IRQ_TYPE_EDGE_BOTH      (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define IRQ_TYPE_LEVEL_HIGH     4
#define IRQ_TYPE_LEVEL_LOW	    8

#endif /* _DT_BINDINGS_IRQ_H_ */
