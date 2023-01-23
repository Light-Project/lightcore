/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

extern void cbfunc_noop(void);
extern long cbfunc_ret0(void);
extern long cbfunc_ret1(void);

#define dummy_noop ((void *)cbfunc_noop)
#define dummy_ret0 ((void *)cbfunc_ret0)
#define dummy_ret1 ((void *)cbfunc_ret1)

#endif /* _CALLBACK_H_ */
