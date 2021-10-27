/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2020 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 */

#include <initcall.h>
#include <mm/page.h>
#include <kshell.h>

struct test {
    char *name;
    int (*fn)(unsigned long volatile *, unsigned long volatile *, size_t);
};

static int compare_regions(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    int r = 0;
    size_t i;
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    off_t physaddr;

    for (i = 0; i < count; i++, p1++, p2++) {
        if (*p1 != *p2) {
            if (use_phys) {
                physaddr = physaddrbase + (i * sizeof(unsigned long));
                fprintf(stderr,
                        "FAILURE: 0x%08lx != 0x%08lx at physical address "
                        "0x%08lx.\n",
                        (unsigned long) *p1, (unsigned long) *p2, physaddr);
            } else {
                fprintf(stderr,
                        "FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n",
                        (unsigned long) *p1, (unsigned long) *p2, (unsigned long) (i * sizeof(unsigned long)));
            }
            /* printf("Skipping to next test..."); */
            r = -1;
        }
    }

    return r;
}

static int test_random_value(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long j = 0;
    size_t i;

    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = rand_ul();
        if (!(i % PROGRESSOFTEN)) {
            putchar('\b');
            putchar(progress[++j % PROGRESSLEN]);
            fflush(stdout);
        }
    }

    return compare_regions(bufa, bufb, count);
}

static int test_xor_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul();
    size_t i;

    for (i = 0; i < count; i++) {
        *p1++ ^= q;
        *p2++ ^= q;
    }

    return compare_regions(bufa, bufb, count);
}

static int test_sub_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul();
    size_t i;

    for (i = 0; i < count; i++) {
        *p1++ -= q;
        *p2++ -= q;
    }

    return compare_regions(bufa, bufb, count);
}

static int test_mul_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul();
    size_t i;

    for (i = 0; i < count; i++) {
        *p1++ *= q;
        *p2++ *= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_div_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul();
    size_t i;

    for (i = 0; i < count; i++) {
        if (!q) {
            q++;
        }
        *p1++ /= q;
        *p2++ /= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_or_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul();
    size_t i;

    for (i = 0; i < count; i++) {
        *p1++ |= q;
        *p2++ |= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_and_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul()
    size_t i;;

    for (i = 0; i < count; i++) {
        *p1++ &= q;
        *p2++ &= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_seqinc_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned long q = rand_ul();
    size_t i;

    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = (i + q);
    }
    return compare_regions(bufa, bufb, count);
}

static int test_solidbits_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j;
    unsigned long q;
    size_t i;

    for (j = 0; j < 64; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? UL_ONEBITS : 0;
        printf("setting %3u", j);
        fflush(stdout);
        p1 = (unsigned long volatile *) bufa;
        p2 = (unsigned long volatile *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

static int test_checkerboard_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j;
    unsigned long q;
    size_t i;

    for (j = 0; j < 64; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
        printf("setting %3u", j);
        fflush(stdout);
        p1 = (unsigned long volatile *) bufa;
        p2 = (unsigned long volatile *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

int test_blockseq_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < 256; j++) {
        p1 = (unsigned long volatile *) bufa;
        p2 = (unsigned long volatile *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (unsigned long) UL_BYTE(j);
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

static int test_walkbits0_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < UL_LEN * 2; j++) {
        p1 = (unsigned long volatile *) bufa;
        p2 = (unsigned long volatile *) bufb;
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = ONE << j;
            } else { /* Walk it back down. */
                *p1++ = *p2++ = ONE << (UL_LEN * 2 - j - 1);
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

static int test_walkbits1_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < UL_LEN * 2; j++) {
        p1 = (unsigned long volatile *) bufa;
        p2 = (unsigned long volatile *) bufb;
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << j);
            } else { /* Walk it back down. */
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << (UL_LEN * 2 - j - 1));
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

static int test_bitspread_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < UL_LEN * 2; j++) {
        p1 = (unsigned long volatile *) bufa;
        p2 = (unsigned long volatile *) bufb;
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (ONE << j) | (ONE << (j + 2))
                    : UL_ONEBITS ^ ((ONE << j)
                                    | (ONE << (j + 2)));
            } else { /* Walk it back down. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (ONE << (UL_LEN * 2 - 1 - j)) | (ONE << (UL_LEN * 2 + 1 - j))
                    : UL_ONEBITS ^ (ONE << (UL_LEN * 2 - 1 - j)
                                    | (ONE << (UL_LEN * 2 + 1 - j)));
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

static int test_bitflip_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned long volatile *p2 = bufb;
    unsigned int j, k;
    unsigned long q;
    size_t i;

    for (k = 0; k < UL_LEN; k++) {
        q = ONE << k;
        for (j = 0; j < 8; j++) {
            q = ~q;
            p1 = (unsigned long volatile *) bufa;
            p2 = (unsigned long volatile *) bufb;
            for (i = 0; i < count; i++) {
                *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
            }
            if (compare_regions(bufa, bufb, count)) {
                return -1;
            }
        }
    }

    return 0;
}

static int test_8bit_wide_random(unsigned long volatile* bufa, unsigned long volatile* bufb, size_t count) {
    unsigned char volatile *p1, *t;
    unsigned long volatile *p2;
    int attempt;
    unsigned int b, j = 0;
    size_t i;

    for (attempt = 0; attempt < 2;  attempt++) {
        if (attempt & 1) {
            p1 = (unsigned char volatile *) bufa;
            p2 = bufb;
        } else {
            p1 = (unsigned char volatile *) bufb;
            p2 = bufa;
        }
        for (i = 0; i < count; i++) {
            t = mword8.bytes;
            *p2++ = mword8.val = rand_ul();
            for (b=0; b < UL_LEN/8; b++) {
                *p1++ = *t++;
            }
            if (!(i % PROGRESSOFTEN)) {
                putchar('\b');
                putchar(progress[++j % PROGRESSLEN]);
                fflush(stdout);
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

static int test_16bit_wide_random(unsigned long volatile* bufa, unsigned long volatile* bufb, size_t count) {
    unsigned short volatile *p1, *t;
    unsigned long volatile *p2;
    int attempt;
    unsigned int b, j = 0;
    size_t i;

    for (attempt = 0; attempt < 2; attempt++) {
        if (attempt & 1) {
            p1 = (unsigned short volatile *) bufa;
            p2 = bufb;
        } else {
            p1 = (unsigned short volatile *) bufb;
            p2 = bufa;
        }
        for (i = 0; i < count; i++) {
            t = mword16.u16s;
            *p2++ = mword16.val = rand_ul();
            for (b = 0; b < UL_LEN/16; b++) {
                *p1++ = *t++;
            }
            if (!(i % PROGRESSOFTEN)) {
                putchar('\b');
                putchar(progress[++j % PROGRESSLEN]);
                fflush(stdout);
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }

    return 0;
}

struct memtest_type memtests[] = {
    { "Random Value", test_random_value },
    { "Compare XOR", test_xor_comparison },
    { "Compare SUB", test_sub_comparison },
    { "Compare MUL", test_mul_comparison },
    { "Compare DIV", test_div_comparison },
    { "Compare OR", test_or_comparison },
    { "Compare AND", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { "Walking Ones", test_walkbits1_comparison },
    { "Walking Zeroes", test_walkbits0_comparison },
    { "8-bit Writes", test_8bit_wide_random },
    { "16-bit Writes", test_16bit_wide_random },
};

static void usage(void)
{
    printk("usage: memtest <mem>[B|K|M|G] [loops]\n");
};

static int bench_main(int argc, char *argv[])
{
    char *para;

    while (--argc) {
        para = *++argv;
        if (*para == '-' && para[1])
        switch (*++para) {
            case 'H':
                proc_halt();
            case 'S':
                power_shutdown();
            case 'r':
                power_restart();
            case 'R':
                power_reset();
            case 'h': default:
                help();
                return 0;
        }
    }

    return 0;
}

static struct kshell_command memtest_cmd = {
    .entry = bench_main,
    .name = "memtest",
    .help = "utility to test for faulty memory"
};

static state memtest_init(void)
{
    return kshell_register(&memtest_cmd);
}
kshell_initcall(memtest_init);
