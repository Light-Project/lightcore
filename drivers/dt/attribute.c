
bool dt_attribute_read_bool(const struct dt_node *node, 
                            const char *name)
{

}

const char *dt_attribute_string_read(const struct dt_node *node, 
                                     const char *name)
{
    struct dt_attribute *attribute;
    attribute = dt_find_attribute(node, name);
    return attribute ? attribute->value : NULL;
}