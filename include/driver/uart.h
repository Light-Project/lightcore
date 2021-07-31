
#ifndef _DRIVER_UART_H_ 
#define _DRIVER_UART_H_

#include <kernel/spinlock.h>

#include <lightcore/uart.h>

/* iotype: uart io type */
#define UPIO_PORT           (UART_IO_PORT)    /* 8b I/O port access */
#define UPIO_HUB6           (UART_IO_HUB6)    /* Hub6 ISA card */
#define UPIO_MEM            (UART_IO_MEM)     /* driver-specific */
#define UPIO_MEM32          (UART_IO_MEM32)   /* 32b little endian */
#define UPIO_AU             (UART_IO_AU)      /* Au1x00 and RT288x type IO */
#define UPIO_TSI            (UART_IO_TSI)     /* Tsi108/109 type IO */
#define UPIO_MEM32BE        (UART_IO_MEM32BE) /* 32b big endian */
#define UPIO_MEM16          (UART_IO_MEM16)   /* 16b little endian */

/* quirks: quirks must be updated while holding port mutex */
#define UPQ_NO_TXEN_TEST    BIT(0)

struct uart_port;

typedef struct uart_ops {

	void (*start_tx)(struct uart_port *);
	void (*stop_tx)(struct uart_port *);
	void (*stop_rx)(struct uart_port *);
    void (*sent_char)(struct uart_port *, char ch);
    void (*startup)(struct uart_port *);  
    void (*shutdown)(struct uart_port *);  
    void (*sync)(struct uart_port *);

} uart_ops_t;

typedef struct uart_port {
    const char              *name;          /* port name */
    spinlock_t              lock;           /* port lock */
    unsigned long           iobase;         /* pmio base */
    unsigned char __iomem   *membase;       /* mmio base */
    unsigned int            irq;            /* irq number */
    unsigned long           irqflags;       /* irq flags  */
    unsigned int            uartclk;        /* uart src clock */
    unsigned int            fifosize;       /* tx fifo size */
    unsigned char           x_char;         /* xon/xoff char */
    unsigned char           regshift;       /* reg offset shift */
    unsigned char           iotype;         /* io access style */
    unsigned char           quirks;         /* internal quirks */
    
    
    int                     (*startup)(struct uart_port *port);         /* uart start */
    void                    (*shutdown)(struct uart_port *port);        /* uart stop */
    unsigned int            (*read)(struct uart_port *, int);           /* uart receive */
    void                    (*write)(struct uart_port *, int, int);     /* uart sent */
    int                     (*handle)(struct uart_port *);       
    
    unsigned int            (*get_divisor)(struct uart_port *,
                            unsigned int baud,
                            unsigned int *frac); 
    void                    (*set_divisor)(struct uart_port *,
                            unsigned int baud,
                            unsigned int quot,
                            unsigned int quot_frac);
    
//     void                    (*set_termios)(struct uart_port *, 
//                             struct ktermios *new, struct ktermios *old);
//     void                    (*set_ldisc)(struct uart_port *,
//                             struct ktermios *);
//     unsigned int            (*get_mctrl)(struct uart_port *);
//     void                    (*set_mctrl)(struct uart_port *, unsigned int);
// 
// 
//     void                    (*throttle)(struct uart_port *port);
//     void                    (*unthrottle)(struct uart_port *port);
//     void                    (*pm)(struct uart_port *, unsigned int state,
//                             unsigned int old);
//     void                    (*handle_break)(struct uart_port *);
//     int                     (*rs485_config)(struct uart_port *,
//                             struct serial_rs485 *rs485);
//     int                     (*iso7816_config)(struct uart_port *,
//                             struct serial_iso7816 *iso7816);


//     unsigned int            read_status_mask;       /* driver specific */
//     unsigned int            ignore_status_mask;     /* driver specific */
//     struct uart_state       *state;                 /* pointer to parent state */
//     struct uart_icount      icount;                 /* statistics */
//     struct console          *cons;                  /* struct console, if any */
//     /* flags must be updated while holding port mutex */
//     upf_t                   flags;

//     /*
//     * These flags must be equivalent to the flags defined in
//     * include/uapi/linux/tty_flags.h which are the userspace definitions
//     * assigned from the serial_struct flags in uart_set_info()
//     * [for bit definitions in the UPF_CHANGE_MASK]
//     *
//     * Bits [0..UPF_LAST_USER] are userspace defined/visible/changeable
//     * The remaining bits are serial-core specific and not modifiable by
//     * userspace.
//     */
// #define UPF_FOURPORT		((__force upf_t) ASYNC_FOURPORT       /* 1  */ )
// #define UPF_SAK			((__force upf_t) ASYNC_SAK            /* 2  */ )
// #define UPF_SPD_HI		((__force upf_t) ASYNC_SPD_HI         /* 4  */ )
// #define UPF_SPD_VHI		((__force upf_t) ASYNC_SPD_VHI        /* 5  */ )
// #define UPF_SPD_CUST		((__force upf_t) ASYNC_SPD_CUST   /* 0x0030 */ )
// #define UPF_SPD_WARP		((__force upf_t) ASYNC_SPD_WARP   /* 0x1010 */ )
// #define UPF_SPD_MASK		((__force upf_t) ASYNC_SPD_MASK   /* 0x1030 */ )
// #define UPF_SKIP_TEST		((__force upf_t) ASYNC_SKIP_TEST      /* 6  */ )
// #define UPF_AUTO_IRQ		((__force upf_t) ASYNC_AUTO_IRQ       /* 7  */ )
// #define UPF_HARDPPS_CD		((__force upf_t) ASYNC_HARDPPS_CD     /* 11 */ )
// #define UPF_SPD_SHI		((__force upf_t) ASYNC_SPD_SHI        /* 12 */ )
// #define UPF_LOW_LATENCY		((__force upf_t) ASYNC_LOW_LATENCY    /* 13 */ )
// #define UPF_BUGGY_UART		((__force upf_t) ASYNC_BUGGY_UART     /* 14 */ )
// #define UPF_MAGIC_MULTIPLIER	((__force upf_t) ASYNC_MAGIC_MULTIPLIER /* 16 */ )
// 
// #define UPF_NO_THRE_TEST	((__force upf_t) (1 << 19))
// /* Port has hardware-assisted h/w flow control */
// #define UPF_AUTO_CTS		((__force upf_t) (1 << 20))
// #define UPF_AUTO_RTS		((__force upf_t) (1 << 21))
// #define UPF_HARD_FLOW		((__force upf_t) (UPF_AUTO_CTS | UPF_AUTO_RTS))
// /* Port has hardware-assisted s/w flow control */
// #define UPF_SOFT_FLOW		((__force upf_t) (1 << 22))
// #define UPF_CONS_FLOW		((__force upf_t) (1 << 23))
// #define UPF_SHARE_IRQ		((__force upf_t) (1 << 24))
// #define UPF_EXAR_EFR		((__force upf_t) (1 << 25))
// #define UPF_BUG_THRE		((__force upf_t) (1 << 26))
// /* The exact UART type is known and should not be probed.  */
// #define UPF_FIXED_TYPE		((__force upf_t) (1 << 27))
// #define UPF_BOOT_AUTOCONF	((__force upf_t) (1 << 28))
// #define UPF_FIXED_PORT		((__force upf_t) (1 << 29))
// #define UPF_DEAD		((__force upf_t) (1 << 30))
// #define UPF_IOREMAP		((__force upf_t) (1 << 31))
// 
// #define __UPF_CHANGE_MASK	0x17fff
// #define UPF_CHANGE_MASK		((__force upf_t) __UPF_CHANGE_MASK)
// #define UPF_USR_MASK		((__force upf_t) (UPF_SPD_MASK|UPF_LOW_LATENCY))
// 
// #if __UPF_CHANGE_MASK > ASYNC_FLAGS
// #error Change mask not equivalent to userspace-visible bit defines
// #endif
// 
//     /*
//     * Must hold termios_rwsem, port mutex and port lock to change;
//     * can hold any one lock to read.
//     */
//     upstat_t		status;
// 
// #define UPSTAT_CTS_ENABLE	((__force upstat_t) (1 << 0))
// #define UPSTAT_DCD_ENABLE	((__force upstat_t) (1 << 1))
// #define UPSTAT_AUTORTS		((__force upstat_t) (1 << 2))
// #define UPSTAT_AUTOCTS		((__force upstat_t) (1 << 3))
// #define UPSTAT_AUTOXOFF		((__force upstat_t) (1 << 4))
// #define UPSTAT_SYNC_FIFO	((__force upstat_t) (1 << 5))
// 
//     int                         hw_stopped;             /* sw-assisted CTS flow state */
//     unsigned int                mctrl;                  /* current modem ctrl settings */
//     unsigned int                timeout;                /* character-based timeout */
//     unsigned int                type;                   /* port type */
    const struct uart_ops       *ops;
//     unsigned int                custom_divisor;
//     unsigned int                line;                   /* port index */
//     unsigned int                minor;
//     resource_size_t             mapbase;                /* for ioremap */
//     resource_size_t             mapsize;
//     struct device               *dev;                   /* parent device */
//     void                        *private_data;          /* generic platform data pointer */
} uart_port_t;

typedef struct uart_driver {
    

} uart_driver_t;

extern int uart_register(struct uart_driver *, struct uart_port *);

#endif
