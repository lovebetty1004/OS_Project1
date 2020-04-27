#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "process.h"
#include "schedule.h"

int main(int argc, char const *argv[])
{
	/* code */
	char sched_type[100];
	int process_num;
	process *p;
	scanf("%s", sched_type);
	scanf("%d", &process_num);
	p = (process *)malloc(process_num * sizeof(process));
	for(int i = 0; i < process_num;i++)
		scanf("%s %d %d", p[i].name, &p[i].ready_t, &p[i].burst_t);
	for(int i = 0; i < process_num;i++)
	 	p[i].start_time = -1;
	for(int i = 0; i < process_num;i++)
	 	p[i].id = i;
	int policy;
	if(strcmp(sched_type, "FIFO") == 0)
		policy = 1;
	else if(strcmp(sched_type, "RR") == 0)
		policy = 2;
	else if(strcmp(sched_type, "SJF") == 0)
		policy = 3;
	else if(strcmp(sched_type, "PSJF") == 0)
		policy = 4;
	else 
	{
		fprintf(stderr, "Invalid policy: %s", sched_type);
		exit(0);
	}
	//printf("%d\n", policy);
	scheduling(p, process_num, policy);
	for(int i= 0; i < process_num;i++)
	{
		fprintf(stderr,"[Project1] pid: %d ",p[i].pid);
		fprintf(stderr,"%lld.%09lld ",p[i].start_time/1000000000ll, p[i].start_time%1000000000ll);
		fprintf(stderr,"%lld.%09lld ",p[i].end_time/1000000000ll, p[i].end_time%1000000000ll);
		fprintf(stderr,"%lld.%09lld ",(p[i].end_time-p[i].start_time)/1000000000ll, (p[i].end_time - p[i].start_time)%1000000000ll);
		fprintf(stderr, "\n");
	}
//}				
	return 0;
}
