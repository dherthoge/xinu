#include <xinu.h>
extern sid32 write_sem_id, read_sem_id, prodcons_bb_sem_id;
extern int32 arr_q[5];
extern int write_idx, read_idx;
void consumer_bb(int count);
void producer_bb(int count);
