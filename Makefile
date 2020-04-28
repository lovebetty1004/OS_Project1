all: main.c process.c schedule.c
	gcc main.c process.c schedule.c -o osp1

clean:
	rm -f osp1