#ifndef _KDEBUG_H_
#define _KDEBUG_H_

typedef enum dump_regs_mode
{
    DUMP_REGS_SHORT,
	/*
	 * For when userspace crashed, but we don't think it's our fault, and
	 * therefore don't print kernel registers.
	 */
	DUMP_REGS_USER,
	DUMP_REGS_ALL,
} dump_regs_mode_t;


#endif
