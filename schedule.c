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
//Queue for RR Linked list
struct queue
{
	int data;
	struct queue *next;
};
typedef struct queue queue;

static queue *readyQ;

void push(int data)
{
	//first insert
	if(readyQ == NULL)
	{
		readyQ = (queue *)malloc(sizeof(queue));
		readyQ -> data = data;
		readyQ -> next = NULL;
		return;
	}
	queue *insert = readyQ;
	while(1)
	{
		if(insert -> next == NULL)
		{
			insert -> next = (queue *)malloc(sizeof(queue));
			insert -> next -> data = data;
			insert -> next -> next = NULL;
			break;
		}
		else
			insert = insert->next;
	}
}
int pop()
{
	if(readyQ == NULL)
		return -1;
	else
	{
		int popoutnumber = readyQ -> data;
		readyQ = readyQ -> next;
		return popoutnumber;
	}
}

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
	//RR
	if(sched_type == 2)
	{
		if(running == -1 || (now_time - rr_last) % 500 == 0)
		{
			int nextq = pop();
			if(nextq == -1)
				t = running;
			else
			{
				//not end yet
				if(running != -1)
				{
					push(running);
				}
				t = nextq;
			}
		}
		else
			t = running;		
	}
	//printf("cs to %d\n", t);
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
	rr_last = 0;
	running = -1;
	finish_count = 0;
	readyQ = NULL;
	
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
				push(i);
				//printf("%d\n", );
				//printf("push = %d\n", i);
				//if(p[i].print_or_not  == 0)
				//{
					//p[i].print_or_not = 1;
				//printf("test = %s %d\n", p[i].name, p[i].pid);
					//printf("%d",p[i].print_or_not);
					
					//printf("%d",p[i].print_or_not);
				//
			}

		}

		/* Select next running  process */
		int next = next_run(p, process_num, sched_type);
		if (next != -1) 
		{
			//printf("next index = %d\n", next);
			/* Context switch */
			if (running != next) 
			{
				//printf("now time = %d next index = %d\n", now_time, next);
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
