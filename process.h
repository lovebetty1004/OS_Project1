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
	int id;
	long long int start_time[2]; 
	long long int end_time[2];
};
typedef struct process process;

void set_CPU(int pid, int affinity);

int create_process(process p);

void set_scheduler(pid_t pid,int priority);
#endif
