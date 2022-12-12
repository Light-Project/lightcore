/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <sched.h>
#include <export.h>

const unsigned int sched_prio_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};

const unsigned int sched_prio_to_wmult[40] = {
 /* -20 */     48388,     59856,     76040,     92818,    118348,
 /* -15 */    147320,    184698,    229616,    287308,    360437,
 /* -10 */    449829,    563644,    704093,    875809,   1099582,
 /*  -5 */   1376151,   1717300,   2157191,   2708050,   3363326,
 /*   0 */   4194304,   5237765,   6557202,   8165337,  10153587,
 /*   5 */  12820798,  15790321,  19976592,  24970740,  31350126,
 /*  10 */  39045157,  49367440,  61356676,  76695844,  95443717,
 /*  15 */ 119304647, 148102320, 186737708, 238609294, 286331153,
};

EXPORT_SYMBOL(sched_prio_to_weight);
EXPORT_SYMBOL(sched_prio_to_wmult);

uint64_t sched_calc_delta(unsigned int dyprio, uint64_t delta)
{
    uint64_t fact = 1024;
    uint32_t upper = upper_32_bits(fact);
    unsigned int offset, shift = 32;

    if (unlikely(upper)) {
        offset = fls(upper);
        fact >>= offset;
        shift -= offset;
    }

    fact = mul_u32_u32(fact, sched_prio_to_wmult[dyprio]);
    upper = upper_32_bits(fact);

    if (upper) {
        offset = fls(upper);
        fact >>= offset;
        shift -= offset;
    }

    return mul_u64_u32_shr(delta, fact, shift);
}
EXPORT_SYMBOL(sched_calc_delta);

uint64_t sched_calc_fair(unsigned int dyprio, uint64_t delta)
{
    if (unlikely(dyprio != 20))
        return sched_calc_delta(dyprio, delta);

    return delta;
}
EXPORT_SYMBOL(sched_calc_fair);
