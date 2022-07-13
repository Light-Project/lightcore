/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _UBSAN_H_
#define _UBSAN_H_

#include <types.h>

enum type_kind {
    type_kind_int   = 0,
    type_kind_float = 1,
    type_unknown    = 0xffff
};

struct type_descriptor {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[1];
};

struct source_location {
    const char *file;
    union {
        unsigned long reported;
        struct {
            uint32_t line;
            uint32_t column;
        };
    };
};

struct overflow {
    struct source_location location;
    struct type_descriptor *type;
};

struct type_mismatch {
    struct source_location location;
    struct type_descriptor *type;
    unsigned long alignment;
    unsigned char type_check_kind;
};

struct type_mismatch_v1 {
    struct source_location location;
    struct type_descriptor *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

struct type_mismatch_common {
    struct source_location *location;
    struct type_descriptor *type;
    unsigned long alignment;
    unsigned char type_check_kind;
};

struct nonnull_arg {
    struct source_location location;
    struct source_location attr_location;
    int arg_index;
};

struct out_of_bounds {
    struct source_location location;
    struct type_descriptor *array_type;
    struct type_descriptor *index_type;
};

struct shift_out_of_bounds {
    struct source_location location;
    struct type_descriptor *lhs_type;
    struct type_descriptor *rhs_type;
};

struct unreachable {
    struct source_location location;
};

struct invalid_value {
    struct source_location location;
    struct type_descriptor *type;
};

struct alignment_assumption {
    struct source_location location;
    struct source_location assumption_location;
    struct type_descriptor *type;
};

extern void __ubsan_handle_divrem_overflow(struct overflow *data, void *lhs, void *rhs);
extern void __ubsan_handle_type_mismatch(struct type_mismatch *data, void *ptr);
extern void __ubsan_handle_type_mismatch_v1(struct type_mismatch_v1 *data, void *ptr);
extern void __ubsan_handle_out_of_bounds(struct out_of_bounds *data, void *index);
extern void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds *data, void *lhs, void *rhs);
extern void __ubsan_handle_builtin_unreachable(struct unreachable *data);
extern void __ubsan_handle_load_invalid_value(struct invalid_value *data, void *val);
extern void __ubsan_handle_alignment_assumption(struct alignment_assumption *data, uintptr_t ptr, uintptr_t align, uintptr_t offset);

#endif  /* _UBSAN_H_ */
