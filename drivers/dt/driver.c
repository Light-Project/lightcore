#include <driver/dt.h>
#include <driver/dt/libfdt.h>



static int dt_node_match_one(struct dt_device_id *did, struct dt_node *dnod)
{


}

struct dt_device_id *dt_node_match(struct dt_device_id *did, struct dt_node *dnod)
{
    struct dt_device_id *bdid = NULL;
    int best = 0;

    while(did->name[0] || did->type[0] || did->compatible[0]) {
        if(dt_node_match_one(did, dnod))
            return did;
        ++did;
    }
    return bdid;
}
