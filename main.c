#include <linux/module.h>
#include <linux/init.h>

int Code_init(void)
{
       return 0;
}

void Code_exit(void)
{
       return;
}

module_init(Code_init);  
module_exit(Code_exit);  
