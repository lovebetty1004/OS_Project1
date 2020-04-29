#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>

asmlinkage void sys_my_gettime(long long int *sec, long long int *nsec)
{
	printk("my_gettime is invoked!\n");
	struct timespec t;
	getnstimeofday(&t);
	*sec = t.tv_sec;
	*nsec = t.tv_nsec;
	return;
} 
