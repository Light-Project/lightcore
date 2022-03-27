#include <kshell.h>
#include <initcall.h>
#include <printk.h>
static state gui_main( int argc, char *argv[]){
    printk("芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖芜湖\n\n\n\n");
    int sum=0;
	int i=0;
	while(i<101)
	{
		sum+=i;
		i++;
	}
	printk("100 %d  \n\n\n",sum);
   return -ENOERR;
}

static struct kshell_command gui_cmd = {
    .name = "gui",
    .desc = "a gui program",
    .exec = gui_main,
};

static state gui_init(void)
{
    return kshell_register(&gui_cmd);
}
kshell_initcall(gui_init);
