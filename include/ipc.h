/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IPC_H_
#define _IPC_H_

#include <namespace.h>

struct ipc_type {
    state (*create)(struct namespace_ipc *, int flags);
};

#endif /* _IPC_H_ */
