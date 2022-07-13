/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ubsan.h>
#include <sched.h>
#include <printk.h>
#include <export.h>

#define VALUE_LENGTH 40
#if (BITS_PER_LONG == 64) && defined(CONFIG_ARCH_BIG_ENDIAN)
#define LINE_MASK   UINT32_MAX
#define COLUMN_MASK INT32_MAX
#else
#define LINE_MASK   INT32_MAX
#define COLUMN_MASK UINT32_MAX
#endif

static const char *const type_check_kinds[] = {
    "load of",
    "store to",
    "reference binding to",
    "member access within",
    "member call on",
    "constructor call on",
    "downcast of",
    "downcast of"
};

static __always_inline bool
ubsan_suppress(void)
{
    return !!current->ubsan_depth;
}

static __always_inline bool
type_is_int(struct type_descriptor *type)
{
    return type->type_kind == type_kind_int;
}

static __always_inline bool
type_is_signed(struct type_descriptor *type)
{
    return type->type_info & 1;
}

static __always_inline unsigned int
type_bit_width(struct type_descriptor *type)
{
    return BIT(type->type_info >> 1);
}

static __always_inline bool
type_inline_int(struct type_descriptor *type)
{
    return type_bit_width(type) <= BITS_PER_LONG;
}

static intmax_t type_get_signed(struct type_descriptor *type, intmax_t *value)
{
    if (type_inline_int(type))
        return (intptr_t)((uintptr_t)value & BIT_LOW_MASK(type_bit_width(type)));
    else if (type_bit_width(type) == BITS_PER_U64)
        return *(int64_t *)value;
    else
        return *value;
}

static uintmax_t type_get_unsigned(struct type_descriptor *type, uintmax_t *value)
{
    if (type_inline_int(type))
        return (uintptr_t)value;
    else if (type_bit_width(type) == BITS_PER_U64)
        return *(uint64_t *)value;
    else
        return *value;
}

static int value_sprintf(struct type_descriptor *type, char *buff,
                         size_t size, void *value)
{
    if (!type_is_int(type))
        return 0;

    if (type_bit_width(type) == 128) {
#ifdef CONFIG_ARCH_HAS_INT128
        uintmax_t getval = type_get_unsigned(type, value);
        return umaxntoa(getval, buff, 10, size);
#else
        return 0;
#endif
    } else if (type_is_signed(type)) {
        int64_t getval = type_get_signed(type, value);
        return s64ntoa(getval, buff, 10, size);
    } else {
        uint64_t getval = type_get_unsigned(type, value);
        return s64ntoa(getval, buff, 10, size);
    }
}

static void ubsan_entry(struct source_location *loc, const char *handle)
{
    current->ubsan_depth++;
    pr_alert("Ubsan Report: %s in %s:%d:%d\n", handle,
             loc->file, loc->line & LINE_MASK, loc->column & COLUMN_MASK);
}

static void ubsan_leave(void)
{
    pr_alert("---[ end ubsan report ]---\n");
    current->ubsan_depth--;
}

static void ubsan_null_ptr_deref(struct type_mismatch_common *data)
{
    ubsan_entry(data->location, "null-ptr-deref");
    pr_alert("%s null pointer of type %s\n",
              type_check_kinds[data->type_check_kind],
              data->type->type_name);
}

static void ubsan_misaligned_access(struct type_mismatch_common *data, void *ptr)
{
    ubsan_entry(data->location, "misaligned-access");
    pr_alert("%s misaligned address %p for type %s\n",
              type_check_kinds[data->type_check_kind],
              ptr, data->type->type_name);
    pr_alert("which requires %ld byte alignment\n", data->alignment);
}

static void ubsan_object_size_mismatch(struct type_mismatch_common *data, void *ptr)
{
    ubsan_entry(data->location, "object-size-mismatch");
    pr_alert("%s address %p with insufficient space\n",
              type_check_kinds[data->type_check_kind], ptr);
    pr_alert("for an object of type %s\n", data->type->type_name);
}

static void ubsan_handle_mismatch(struct type_mismatch_common *data, void *ptr)
{
    if (ubsan_suppress())
        return;
    if (ptr == NULL)
        ubsan_null_ptr_deref(data);
    else if (data->alignment && !align_ptr_check(ptr, data->alignment))
        ubsan_misaligned_access(data, ptr);
    else
        ubsan_object_size_mismatch(data, ptr);
    ubsan_leave();
}

void __ubsan_handle_divrem_overflow(struct overflow *data, void *lhs, void *rhs)
{
    char buff[VALUE_LENGTH];
    int length;

    if (ubsan_suppress())
        return;

    ubsan_entry(&data->location, "division-overflow");
    if (!type_is_signed(data->type) || type_get_signed(data->type, rhs) != -1)
        pr_alert("division by zero\n");
    else {
        length = value_sprintf(data->type, buff, VALUE_LENGTH, rhs);
        pr_alert("load of value %.*s is not a valid value for type %s\n",
                  length, buff, data->type->type_name);
    }
    ubsan_leave();
}
EXPORT_SYMBOL(__ubsan_handle_divrem_overflow);

void __ubsan_handle_type_mismatch(struct type_mismatch *data, void *ptr)
{
    struct type_mismatch_common common_data = {
        .location = &data->location,
        .type = data->type,
        .alignment = data->alignment,
        .type_check_kind = data->type_check_kind
    };
    ubsan_handle_mismatch(&common_data, ptr);
}
EXPORT_SYMBOL(__ubsan_handle_type_mismatch);

void __ubsan_handle_type_mismatch_v1(struct type_mismatch_v1 *data, void *ptr)
{
    struct type_mismatch_common common_data = {
        .location = &data->location,
        .type = data->type,
        .alignment = BIT(data->log_alignment),
        .type_check_kind = data->type_check_kind
    };
    ubsan_handle_mismatch(&common_data, ptr);
}
EXPORT_SYMBOL(__ubsan_handle_type_mismatch_v1);

void __ubsan_handle_out_of_bounds(struct out_of_bounds *data, void *index)
{
    char buff[VALUE_LENGTH];
    int length;

    if (ubsan_suppress())
        return;

    ubsan_entry(&data->location, "array-index-out-of-bounds");
    length = value_sprintf(data->index_type, buff, VALUE_LENGTH, index);
    pr_alert("index %.*s is out of range for type %s\n",
              length, buff, data->array_type->type_name);
    ubsan_leave();
}
EXPORT_SYMBOL(__ubsan_handle_out_of_bounds);

void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds *data, void *lhs, void *rhs)
{
    char lbuff[VALUE_LENGTH], rbuff[VALUE_LENGTH];
    int llength, rlength;

    if (ubsan_suppress())
        return;

    ubsan_entry(&data->location, "shift-out-of-bounds");
    llength = value_sprintf(data->lhs_type, lbuff, VALUE_LENGTH, lhs);
    rlength = value_sprintf(data->rhs_type, rbuff, VALUE_LENGTH, rhs);
    if (type_is_signed(data->rhs_type) && type_get_signed(data->rhs_type, rhs) < 0)
        pr_alert("shift exponent %.*s is negative\n", rlength, rbuff);
    else if (type_get_unsigned(data->rhs_type, rhs) >= type_bit_width(data->rhs_type))
        pr_alert("shift exponent %.*s is too large for %u-bit type %s\n",
                 rlength, rbuff, type_bit_width(data->rhs_type), data->rhs_type->type_name);
    else if (type_is_signed(data->rhs_type) && type_get_signed(data->rhs_type, rhs) < 0)
        pr_alert("left shift of negative value %.*s\n", llength, lbuff);
	else
		pr_alert("left shift of %.*s by %.*s places cannot be represented in type %s\n",
			      llength, lbuff, rlength, rbuff, data->lhs_type->type_name);
    ubsan_leave();
}
EXPORT_SYMBOL(__ubsan_handle_shift_out_of_bounds);

void __ubsan_handle_builtin_unreachable(struct unreachable *data)
{
    if (ubsan_suppress())
        return;
    ubsan_entry(&data->location, "unreachable");
    pr_alert("builtin-unreachable occur\n");
    ubsan_leave();
}
EXPORT_SYMBOL(__ubsan_handle_builtin_unreachable);

void __ubsan_handle_load_invalid_value(struct invalid_value *data, void *val)
{
    char buff[VALUE_LENGTH];
    int length;
    if (ubsan_suppress())
        return;
    ubsan_entry(&data->location, "invalid-load");
    length = value_sprintf(data->type, buff, VALUE_LENGTH, val);
    pr_alert("load of value %.*s is not a valid value for type %s\n",
              length, buff, data->type->type_name);
    ubsan_leave();
}
EXPORT_SYMBOL(__ubsan_handle_load_invalid_value);

void __ubsan_handle_alignment_assumption(struct alignment_assumption *data, uintptr_t ptr,
                                         uintptr_t align, uintptr_t offset)
{
    uintptr_t address = ptr - offset;
    if (ubsan_suppress())
        return;
    ubsan_entry(&data->location, "alignment-assumption");
    if (offset)
        pr_alert("assumption of %lu byte alignment (with offset of %lu byte) for pointer of type %s failed\n",
                  align, offset, data->type->type_name);
    else
        pr_alert("assumption of %lu byte alignment for pointer of type %s failed\n",
                  align, data->type->type_name);
    pr_alert("%saddress is %lu aligned, misalignment offset is %lu bytes\n",
              offset ? "offset " : "", BIT(address ? ffsuf(address) : 0), address & (align - 1));
    ubsan_leave();
}
EXPORT_SYMBOL(__ubsan_handle_alignment_assumption);
