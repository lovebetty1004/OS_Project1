#define _GNU_SOURCE
#include "process.h"
#include "schedule.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
/* Last context switch time for RR scheduling */
static int t_last;

/* Current unit time */
static int ntime;

/* Index of running process. -1 if no process running */
static int running;

/* Number of finish Process */
static int finish_cnt;

int compare(const void *ptr1, const void *ptr2)
{
	process *a = (process *)ptr1;
	process *b = (process *)ptr2;
	if(a->ready_t < b->ready_t)
		return -1;
	if(a ->ready_t > b->ready_t)
		return 1;
	if(a -> id < b -> id)
		return -1;
	if(a -> id > b-> id)
		return 1;
	return 0;
}
int next_run(process *p, int process_num, int policy)
{
	if(running != -1 && policy == 1)
		return running;

	int t = -1;
	if(policy == 1)
	{
		for(int i = 0; i < process_num; i++)
		{
			if(p[i].pid == -1)
				continue;
			if(p[i].burst_t == 0)
				continue;
			if(t == -1|| p[i].ready_t < p[t].ready_t)
				t = i;
		}
	}
	return t;
}
int scheduling(process *p, int process_num, int policy)
{
	qsort(p, process_num, sizeof(process), compare);
	
	for(int i = 0; i < process_num; i++)
		p[i].pid = -1;

	set_CPU(getpid(), 0);
	
	/* Set high priority to scheduler */
	set_scheduler(getpid(),99);

	/* Initial scheduler */
	ntime = 0;
	running = -1;
	finish_cnt = 0;
	
	while(1) {
		//fprintf(stderr, "Current time: %d\n", ntime);

		/* Check if running process finish */
		if (running != -1 && p[running].burst_t == 0) 
		{

			//kill(running, SIGKILL);
			//
			waitpid(p[running].pid, NULL, 0);
			p[running].end_time = cur_time();
			fprintf(stderr,"%lld.%09lld \n",p[running].end_time/1000000000ll, p[running].end_time%1000000000ll);
			//printf("%s %d\n", p[running].name, p[running].pid);
			running = -1;
			finish_cnt++;

			/* All process finish */
			if (finish_cnt == process_num)
				break;
		}

		/* Check if process ready and execute */
		for (int i = 0; i < process_num; i++) 
		{
			if (p[i].ready_t == ntime) 
			{
				p[i].pid = create_process(p[i]);
				set_scheduler(p[i].pid, 1);
				printf("%s %d\n", p[i].name, p[i].pid);
			}

		}

		/* Select next running  process */
		int next = next_run(p, process_num, policy);
		if (next != -1) 
		{
			/* Context switch */
			if (running != next) 
			{
				set_scheduler(p[next].pid,99);
				p[next].start_time = cur_time();
				fprintf(stderr,"[Project1] pid: %d ",p[next].pid);
				fprintf(stderr,"%lld.%09lld ",p[next].start_time/1000000000ll, p[next].start_time%1000000000ll);
				
				set_scheduler(p[running].pid, 1);
				running = next;
				t_last = ntime;
			}
		}

		/* Run an unit of time */
		unit_time();
		if (running != -1)
			p[running].burst_t--;
		ntime++;
	}
	return 0;

}
