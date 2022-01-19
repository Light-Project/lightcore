/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_BUG_H_
#define _COMPILER_BUG_H_

/*
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type int), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted).
 */
#define BUILD_ASSERT(e) ((int)(sizeof(struct { int:(-!!(e)); })))

#endif  /* _COMPILER_BUG_H_ */
