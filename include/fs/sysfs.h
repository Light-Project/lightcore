
#include <kobject.h>

#ifdef CONFIG_SYSFS

#else

static inline int sysfs_dir_create_ns(struct kobject *kobj, const void *ns)
{
    return 0;
}

static inline void sysfs_dir_remove_ns(struct kobject *kobj)
{
}

static inline int sysfs_dir_rename_ns(struct kobject *kobj, struct kobject *new_name, const void *new_ns)
{
    return 0;
}

static inline int sysfs_dir_move_ns(struct kobject *kobj, struct kobject *new_parent_kobj, const void *new_ns)
{
    return 0;
}

static inline sysfs_file_create_ns()
{
}

static inline sysfs_file_create_ns()
{
}

#endif

static inline int __must_check sysfs_create_file(struct kobject *kobj, const struct attribute *attr)
{
    return sysfs_create_file_ns(kobj, attr, NULL);
}

static inline void sysfs_remove_file(struct kobject *kobj, const struct attribute *attr)
{
    sysfs_remove_file_ns(kobj, attr, NULL);
}

static inline int sysfs_rename_link(struct kobject *kobj, struct kobject *target, const char *old_name, const char *new_name)
{
    return sysfs_rename_link_ns(kobj, target, old_name, new_name, NULL);
}

static inline void sysfs_notify_dirent(struct kernfs_node *kn)
{
    kernfs_notify(kn);
}

static inline struct kernfs_node *sysfs_get_dirent(struct kernfs_node *parent,
                        const char *name)
{
    return kernfs_find_and_get(parent, name);
}

static inline struct kernfs_node *sysfs_get(struct kernfs_node *kn)
{
    kernfs_get(kn);
    return kn;
}

static inline void sysfs_put(struct kernfs_node *kn)
{
    kernfs_put(kn);
}
