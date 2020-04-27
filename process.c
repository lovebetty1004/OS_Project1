#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#include "process.h"

long long int cur_time(){
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t.tv_sec * 1000000000ll + t.tv_nsec;
}

void unit_time()				
{						
	volatile unsigned long i;		
	for (i = 0; i < 1000000UL; i++);	
}	
void set_CPU(int pid, int affinity)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(affinity, &mask);
	if(sched_setaffinity(pid, sizeof(mask), &mask) < 0)
	{
		perror("sched_setaffinity error");
		exit(EXIT_FAILURE);
	}
	return;
}
int create_process(process p)
{
	int pid = fork();
	//p.start_time = cur_time();
	if(pid < 0)
	{
		perror("fork error");
		return -1;
	}
	else if(pid == 0)
	{
		// fprintf(stderr,"%lld.%09lld ",start_time/1000000000ll, start_time%1000000000ll);
		
		//long long int end_timeinchild;
		//long long int run_time;
		
		// printf("now time = %lld\n", cur_time());
		for(int i = 0; i < p.burst_t;i++)
			unit_time();
		//end_timeinchild = cur_time();
		//run_time = end_time - p.start_time;
		// printf("now end time = %lld\n", cur_time());
		
		//fprintf(stderr,"%lld.%09lld ",p.start_time/1000000000ll, p.start_time%1000000000ll);
		//fprintf(stderr,"end time in chile:%lld.%09lld\n ",end_timeinchild/1000000000ll, end_timeinchild%1000000000ll);
		// fprintf(stderr,"%lld.%09lld ",d_time/1000000000ll, d_time%1000000000ll);
		//fprintf(stderr, "%lld.%09lld", run_time/1000000000ll, run_time%1000000000ll);
		//fprintf(stderr,"\n");
		exit(0);
	}
	set_CPU(pid, 1);
	return pid;
}
//wake_up and block 
// int proc_block(int pid)
// {
// 	struct sched_param param;
	
// 	/* SCHED_IDLE should set priority to 0 */
// 	param.sched_priority = 0;

// 	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	
// 	if (ret < 0) {
// 		perror("sched_setscheduler error");
// 		return -1;
// 	}

// 	return ret;
// }

// int proc_wakeup(int pid)
// {
// 	struct sched_param param;
	
// 	/* SCHED_OTHER should set priority to 0 */
// 	param.sched_priority = 0;

// 	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	
// 	if (ret < 0) {
// 		perror("sched_setscheduler error");
// 		return -1;
// 	}

// 	return ret;
// }
void set_scheduler(pid_t pid,int priority)
{
    struct sched_param param;
    param.sched_priority = priority;
    if(sched_setscheduler(pid,SCHED_FIFO,&param)!=0)
    {
        perror("sched_setscheduler error");
        exit(EXIT_FAILURE);
    }
}
