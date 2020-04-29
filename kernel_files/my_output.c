#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_my_output(char *info)
{
	printk("%s\n", info);
	return ;
}

