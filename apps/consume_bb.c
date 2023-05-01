#include <xinu.h>
#include <prodcons_bb.h>

void consumer_bb(int count) {
	for (int i = 0; i < count; i++) {
		wait(write_sem_id);
		int val = arr_q[read_idx % 5];
		read_idx++;
		char *proc_name = proctab[getpid()].prname;
		printf("name : %s, read : %d\n", proc_name, val);
		signal(read_sem_id);
	}
	
	//semdelete(write_sem_id);
	//semdelete(read_sem_id);
	signal(prodcons_bb_sem_id);
}
