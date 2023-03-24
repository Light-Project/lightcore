/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DT_HELP_H_
#define _DRIVER_DT_HELP_H_

#define GENERIC_DT_ATTRIBUTE_VALUE_ARRAY(func, type)                                \
static inline state dt_attribute_read_##func##_index_array                          \
(const struct dt_node *node, const char *name, type *buff, int index, size_t min)   \
{                                                                                   \
    int length;                                                                     \
    length = dt_attribute_read_##func##_index_array_range                           \
    (node, name, buff, index, min, 0);                                              \
    return min(length, 0);                                                          \
}                                                                                   \
                                                                                    \
static inline state dt_attribute_read_##func##_array                                \
(const struct dt_node *node, const char *name, type *buff, size_t min)              \
{                                                                                   \
    return dt_attribute_read_##func##_index_array(node, name, buff, 0, min);        \
}

GENERIC_DT_ATTRIBUTE_VALUE_ARRAY(u8, uint8_t)
GENERIC_DT_ATTRIBUTE_VALUE_ARRAY(u16, uint16_t)
GENERIC_DT_ATTRIBUTE_VALUE_ARRAY(u32, uint32_t)
GENERIC_DT_ATTRIBUTE_VALUE_ARRAY(u64, uint64_t)

static inline state dt_attribute_read_u32(const struct dt_node *node, const char *name, uint32_t *value)
{
    return dt_attribute_read_u32_index(node, name, 0, value);
}

static inline state dt_attribute_read_u64(const struct dt_node *node, const char *name, uint64_t *value)
{
    return dt_attribute_read_u64_index(node, name, 0, value);
}

static inline state dt_attribute_read_string_index(const struct dt_node *node, const char *name, unsigned int index, const char **str)
{
    return dt_attribute_read_string_index_array(node, name, str, index, 1);
}

static inline state dt_attribute_read_string(const struct dt_node *node, const char *name, const char **str)
{
    return dt_attribute_read_string_index(node, name, 0, str);
}

static inline state dt_parse_phandle_with_args(const struct dt_node *node, const char *list_name, const char *cells_name, int index, struct dt_phandle_args *args)
{
    int cell_count = cells_name ? -1 : 0;
    return dt_parse_phandle_with_args_count(node, list_name, cells_name, cell_count, index, args);
}

#endif /* _DRIVER_DT_HELP_H_ */
