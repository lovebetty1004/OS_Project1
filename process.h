#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

long long int cur_time();

void unit_time();			
						
struct process 
{
	char name[100];
	int ready_t;
	int burst_t;
	pid_t pid;
};
typedef struct process process;

void set_CPU(int pid, int affinity);

int create_process(process p);

// /* Set very low priority tp process */
// int proc_block(int pid);

// /* Set high priority to process */
// int proc_wakeup(int pid);

void set_scheduler(pid_t pid,int priority);
#endif