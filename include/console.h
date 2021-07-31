#ifndef _DRIVER_CONSOLE_H_
#define _DRIVER_CONSOLE_H_

#include <slist.h>



#define CON_PRINTBUFFER	(1)
#define CON_CONSDEV 	(2) /* Preferred console, /dev/console */
#define CON_ENABLED	    (4)
#define CON_BOOT	    (8)
#define CON_ANYTIME	    (16) /* Safe to call when cpu is offline */
#define CON_BRL		    (32) /* Used for a braille device */
#define CON_EXTENDED	(64) /* Use the extended output format a la /dev/kmsg */

typedef struct console {
    char    name[16];
    slist_t list;       /* console slist */
    
    int     (*setup)(struct console *, char *);                 /* entry console */
    int     (*exit)(struct console *);                          /* exit console */
    int     (*read)(struct console *, char *, unsigned);        /* console receive */
    void    (*write)(struct console *, const char *, unsigned); /* console sent */
    
    short   index;
    void    *data;  /* Private data */

    uint16_t    enable:1;
    uint16_t    boot:1;
} console_t; 

void console_lock(void);
void console_unlock(void);

void console_write(const char *str, unsigned int len);

extern void console_register(console_t *);
extern void console_unregister(console_t *);
extern void console_init(void);

#endif
