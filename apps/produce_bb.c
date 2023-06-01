#include <xinu.h>
#include <prodcons_bb.h>

void producer_bb(int count) {
	for (int i = 0; i < count; i++) {
		wait(read_sem_id);
		arr_q[write_idx % 5] = i;
		write_idx++;
		char *proc_name = proctab[getpid()].prname;
		printf("name : %s, write : %d\n", proc_name, i);		
		signal(write_sem_id);
	}
}
