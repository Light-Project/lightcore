/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KGCOV_GCC_H_
#define _KGCOV_GCC_H_

#if (__GNUC__ >= 10)
#define KGCOV_COUNTERS  8
#elif (__GNUC__ >= 7)
#define KGCOV_COUNTERS  9
#elif (__GNUC__ > 5) || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)
#define KGCOV_COUNTERS  10
#else
#define KGCOV_COUNTERS  9
#endif

struct kgcov_ctr_info {
    unsigned int num;
    kgcov_t *values;
};

struct kgcov_fn_info {
    const struct gcov_info *key;
    unsigned int ident;
    unsigned int lineno_checksum;
    unsigned int cfg_checksum;
    struct kgcov_ctr_info ctrs[];
};

struct kgcov_info {
    unsigned int version;
    struct kgcov_info *next;
    unsigned int stamp;
    const char *filename;
    void (*merge[KGCOV_COUNTERS])(kgcov_t *, unsigned int);
    unsigned int n_functions;
    struct kgcov_fn_info **functions;
};

extern const char *kgcov_info_filename(struct kgcov_info *info);
extern unsigned int kgcov_info_version(struct kgcov_info *info);
extern void kgcov_info_link(struct kgcov_info *info);
extern void kgcov_info_unlink(struct kgcov_info *prev, struct kgcov_info *info);

#endif /* _KGCOV_GCC_H_ */
