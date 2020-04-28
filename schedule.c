#define _GNU_SOURCE
#include "process.h"
#include "schedule.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

//Last running = -1 for RR sched
static int previous = -1;
//Last context switch time for RR scheduling 
static int rr_last;
//Current unit time
static int now_time;
//Index of running process. -1 if no process running 
static int running;
//Number of finish Process 
static int finish_count;

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
int next_run(process *p, int process_num, int sched_type)
{
	//Non-preemptive
	if(running != -1 && sched_type == 1)
		return running;
	else if(running != -1 && sched_type == 3)
		return running;

	int t = -1;
	//FIFO
	if(sched_type == 1)
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
	//SJF
	if(sched_type == 3)
	{
		for(int i = 0; i < process_num; i++)
		{
			if(p[i].pid == -1)
				continue;
			if(p[i].burst_t <= 0)
				continue;
			if(t == -1|| p[i].burst_t < p[t].burst_t)
				t = i;
		}
	}
	//PSJF
	if(sched_type == 4)
	{
		for(int i = 0; i < process_num; i++)
		{
			if(p[i].pid == -1)
				continue;
			if(p[i].burst_t <= 0)
				continue;
			if(t == -1|| p[i].burst_t < p[t].burst_t)
				t = i;
		}
	}
	if(sched_type == 2)
	{	
		
		if(running == -1)
		{	if(previous != -1)
			{
				t = previous+1;
				if(t >= process_num)
					t = 0;
				//t = (running+1)%process_num; 
				while(p[t].pid == -1 || p[t].burst_t <= 0)
				{
					t++;
					if(t >= process_num)
						t = 0;
				//t = (t+1)%process_num;  
				}
			}
			else
			{
				for(int i = 0; i < process_num;i++)
				{
					if(p[i].pid != -1 && p[i].burst_t > 0)
					{
						t = i;
						break;
					}
				}
			}
			
		}

		else if((now_time - rr_last) % 500 == 0)
		{
			t = running +1;
			if(t >= process_num)
				t = 0;
			
			while(p[t].pid == -1 || p[t].burst_t <= 0)
			{
				t++;
				if(t >= process_num)
					t = 0;
				//t = (t+1)%process_num;  
			}
			//fprintf(stderr, "n-t: %d %d\n", now_time, t_last) ;
		}
		else
			t = running;
	}
	return t;
}
int scheduling(process *p, int process_num, int sched_type)
{
	qsort(p, process_num, sizeof(process), compare);
	
	for(int i = 0; i < process_num; i++)
		p[i].pid = -1;

	set_CPU(getpid(), 0);
	
	//Set high priority to scheduler 
	set_scheduler(getpid(),99);

	now_time = 0;
	running = -1;
	finish_count = 0;
	
	while(1) {
		//fprintf(stderr, "Current time: %d\n", now_time);
		//fprintf(stderr, "running =  %d %d\n", running, p[running].burst_t);
		/* Check if running process finish */
		if (running != -1 && p[running].burst_t == 0) 
		{

			//kill(running, SIGKILL);
			//
			waitpid(p[running].pid, NULL, 0);
			syscall(334, &p[running].end_time[0], &p[running].end_time[1]);
			
			//fprintf(stderr,"%d %lld.%09lld \n",running, p[running].end_time/1000000000ll, p[running].end_time%1000000000ll);
			//printf("%s %d\n", p[running].name, p[running].pid);
			previous = running;			
			running = -1;
			finish_count++;

			/* All process finish */
			if (finish_count == process_num)
				break;
		}

		/* Check if process ready and execute */
		for (int i = 0; i < process_num; i++) 
		{
			if (p[i].ready_t == now_time) 
			{
				p[i].pid = create_process(p[i]);
				set_scheduler(p[i].pid, 1);
				printf("%s %d\n", p[i].name, p[i].pid);
			}

		}

		/* Select next running  process */
		int next = next_run(p, process_num, sched_type);
		if (next != -1) 
		{
			/* Context switch */
			if (running != next) 
			{
				set_scheduler(p[next].pid,99);
				if(p[next].start_time[0] == -1)
				//{
					//p[next].start_time = cur_time();
					syscall(334,&p[next].start_time[0], &p[next].start_time[1]);
					//fprintf(stderr,"[Project1] pid: %d ",p	[next].pid);

					//fprintf(stderr,"%lld.%09lld ",p[next].start_time/1000000000ll, p[next].start_time%1000000000ll);
//}				
				set_scheduler(p[running].pid, 1);
				running = next;
				rr_last = now_time;
			}
		}

		/* Run an unit of time */
		unit_time();
		if (running != -1)
			p[running].burst_t--;
		now_time++;
	}
	return 0;

}
