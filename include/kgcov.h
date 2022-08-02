/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KGCOV_H_
#define _KGCOV_H_

#if BITS_PER_LONG >= 64
typedef long kgcov_t;
#else
typedef long long kgcov_t;
#endif

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

/**
 * gcov_info_filename - return info filename.
 * @info: profiling data set.
 */
static inline const char *gcov_info_filename(struct kgcov_info *info)
{
	return info->filename;
}

/**
 * gcov_info_version - return info version.
 * @info: profiling data set.
 */
static inline unsigned int gcov_info_version(struct kgcov_info *info)
{
	return info->version;
}

#endif  /* _KGCOV_H_ */
