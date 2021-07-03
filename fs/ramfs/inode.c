 
#include <init/initcall.h>
 

static void ramfs_kill_sb()
{
    
    
    
}

struct filesystem_type ramfs_type = 
{
    .name = "ramfs",
    
    
}
 
static state ramfs_register()
{
    return filesystem_register();
}
 
fs_initcall(ramfs_register);
