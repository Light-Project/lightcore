/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TERMIOS_H_
#define _TERMIOS_H_

/* c_iflag bits */
#define IGNBRK      0000001 /* Ignore break condition. */
#define BRKINT      0000002 /* Signal interrupt on break. */
#define IGNPAR      0000004 /* Ignore characters with parity errors. */
#define PARMRK      0000010 /* Mark parity and framing errors. */
#define INPCK       0000020 /* Enable input parity check. */
#define ISTRIP      0000040 /* Strip 8th bit off characters. */
#define INLCR       0000100 /* Map NL to CR on input. */
#define IGNCR       0000200 /* Ignore CR. */
#define ICRNL       0000400 /* Map CR to NL on input. */
#define IUCLC       0001000 /* Map uppercase characters to lowercase on input (not in POSIX). */
#define IXON        0002000 /* Enable start/stop output control. */
#define IXANY       0004000 /* Enable any character to restart output. */
#define IXOFF       0010000 /* Enable start/stop input control. */
#define IMAXBEL     0020000 /* Ring bell when input queue is full (not in POSIX). */
#define IUTF8       0040000 /* Input is UTF8 (not in POSIX). */

/* c_oflag bits */
#define OPOST       0000001 /* Post-process output. */
#define OLCUC       0000002 /* Map lowercase characters to uppercase on output. (not in POSIX).*/
#define ONLCR       0000004 /* Map NL to CR-NL on output. */
#define OCRNL       0000010 /* Map CR to NL on output. */
#define ONOCR       0000020 /* No CR output at column 0. */
#define ONLRET      0000040 /* NL performs CR function. */
#define OFILL       0000100 /* Use fill characters for delay. */
#define OFDEL       0000200 /* Fill is DEL. */
#define NLDLY       0000400 /* Select newline delays: */
#define NL0         0000000 /* Newline type 0. */
#define NL1         0000400 /* Newline type 1. */
#define CRDLY       0003000 /* Select carriage-return delays: */
#define CR0         0000000 /* Carriage-return delay type 0. */
#define CR1         0001000 /* Carriage-return delay type 1. */
#define CR2         0002000 /* Carriage-return delay type 2. */
#define CR3         0003000 /* Carriage-return delay type 3. */
#define TABDLY      0014000 /* Select horizontal-tab delays: */
#define TAB0        0000000 /* Horizontal-tab delay type 0. */
#define TAB1        0004000 /* Horizontal-tab delay type 1. */
#define TAB2        0010000 /* Horizontal-tab delay type 2. */
#define TAB3        0014000 /* Expand tabs to spaces. */
#define BSDLY       0020000 /* Select backspace delays: */
#define BS0         0000000 /* Backspace-delay type 0. */
#define BS1         0020000 /* Backspace-delay type 1. */
#define FFDLY       0100000 /* Select form-feed delays: */
#define FF0         0000000 /* Form-feed delay type 0. */
#define FF1         0100000 /* Form-feed delay type 1. */

/* c_cflag bit */
#define B0          0000000
#define B50         0000001
#define B75         0000002
#define B110        0000003
#define B134        0000004
#define B150        0000005
#define B200        0000006
#define B300        0000007
#define B600        0000010
#define B1200       0000011
#define B1800       0000012
#define B2400       0000013
#define B4800       0000014
#define B9600       0000015
#define B19200      0000016
#define B38400      0000017
#define B57600      0010001
#define B115200     0010002
#define B230400     0010003
#define B460800     0010004
#define B500000     0010005
#define B576000     0010006
#define B921600     0010007
#define B1000000    0010010
#define B1152000    0010011
#define B1500000    0010012
#define B2000000    0010013
#define B2500000    0010014
#define B3000000    0010015
#define B3500000    0010016
#define B4000000    0010017

/* c_cc characters */
#define VINTR       0
#define VQUIT       1
#define VERASE      2
#define VKILL       3
#define VEOF        4
#define VTIME       5
#define VMIN        6
#define VSWTC       7
#define VSTART      8
#define VSTOP       9
#define VSUSP       10
#define VEOL        11
#define VREPRINT    12
#define VDISCARD    13
#define VWERASE     14
#define VLNEXT      15
#define VEOL2       16

/* c_lflag bits */
#define ISIG        0000001 /* Enable signals. */
#define ICANON      0000002 /* Canonical input (erase and kill processing). */
#define XCASE       0000004
#define ECHO        0000010 /* Enable echo. */
#define ECHOE       0000020 /* Echo erase character as error-correctingbackspace. */
#define ECHOK       0000040 /* Echo KILL. */
#define ECHONL      0000100 /* Echo NL. */
#define NOFLSH      0000200 /* Disable flush after interrupt or quit. */
#define TOSTOP      0000400 /* Send SIGTTOU for background output. */
#define ECHOCTL     0001000 /* If ECHO is also set, terminal special characters other than TAB, NL, START, and STOP are echoed as ^X, where X is the character with ASCII code 0x40 greater than the special character (not in POSIX). */
#define ECHOPRT     0002000 /* If ICANON and ECHO are also set, characters are printed as they are being erased (not in POSIX). */
#define ECHOKE      0004000 /* If ICANON is also set, KILL is echoed by erasing each character on the line, as specified by ECHOE and ECHOPRT (not in POSIX). */
#define FLUSHO      0010000 /* Output is being flushed.  This flag is toggled by typing the DISCARD character (not in POSIX). */
#define PENDIN      0040000 /* All characters in the input queue are reprintedwhen the next character is read (not in POSIX). */
#define IEXTEN      0100000 /* Enable implementation-defined inputprocessing. */
#define EXTPROC     0200000

/* tcflow() and TCXONC use these */
#define TCOOFF      0
#define TCOON       1
#define TCIOFF      2
#define TCION       3

/* tcflush() and TCFLSH use these */
#define TCIFLUSH    0
#define TCOFLUSH    1
#define TCIOFLUSH   2

typedef unsigned int tcflag_t;
typedef unsigned int speed_t;
typedef unsigned char cc_t;
#define NCCS 32

/**
 * struct termios - describe termios configuration.
 * c_iflag: input mode flags.
 * c_oflag: output mode flags.
 * c_cflag: control mode flags.
 * c_lflag: local mode flags.
 * c_line: line discipline.
 * c_cc: control characters.
 * c_ispeed: input speed.
 * c_ospeed: output speed.
 */
struct termios {
  tcflag_t c_iflag;
  tcflag_t c_oflag;
  tcflag_t c_cflag;
  tcflag_t c_lflag;
  cc_t c_line;
  cc_t c_cc[NCCS];
  speed_t c_ispeed;
  speed_t c_ospeed;
};

#endif /* _TERMIOS_H_ */
