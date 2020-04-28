#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "process.h"
#include "schedule.h"
//output compare
int cmp(const void *ptr1, const void *ptr2)
{
	process *a = (process *)ptr1;
	process *b = (process *)ptr2;
	if(a->end < b->end)
		return -1;
	if(a ->end > b->end)
		return 1;
	if(a -> id < b -> id)
		return -1;
	if(a -> id > b-> id)
		return 1;
	return 0;
}

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
	 	p[i].start_time[0] = -1;
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
	long long int run_time;
	long long int start;
	long long int end;
	for(int i= 0; i < process_num;i++)
	{
		//fprintf(stderr,"[Project1] pid: %d ",p[i].pid);
		//fprintf(stderr,"%lld.%09lld ",p[i].start_time[0], p[i].start_time[1]);
		//fprintf(stderr,"%lld.%09lld ",p[i].end_time[0], p[i].end_time[1]);
		p[i].start = p[i].start_time[0]*1000000000ll+p[i].start_time[1];
		p[i].end = p[i].end_time[0]*1000000000ll+p[i].end_time[1];
		p[i].run_time = p[i].end-p[i].start;
		
		//fprintf(stderr, "\n");
	}
	qsort(p, process_num, sizeof(process), cmp);
	char dmesg[200];
	for(int i= 0; i < process_num;i++)
	{
		sprintf(dmesg , "[Project1] pid: %d %lld.%09lld %lld.%09lld\n", p[i].pid, p[i].start_time[0], p[i].start_time[1], p[i].end_time[0], p[i].end_time[1]);
		syscall(335, dmesg);
		//fprintf(stderr,"[Project1] pid: %d ",p[i].pid);
		//fprintf(stderr,"%lld.%09lld ",p[i].start_time[0], p[i].start_time[1]);
		//fprintf(stderr,"%lld.%09lld ",p[i].end_time[0], p[i].end_time[1]);
		//p[i].start = p[i].start_time[0]*1000000000ll+p[i].start_time[1];
		//p[i].end = p[i].end_time[0]*1000000000ll+p[i].end_time[1];
		//fprintf(stderr,"%lld.%09lld ",p[i].run_time/1000000000ll, p[i].run_time%1000000000ll);
		
		//fprintf(stderr, "\n");
	}
//}				
	return 0;
}
