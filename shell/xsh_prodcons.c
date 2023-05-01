#include <xinu.h>
#include <prodcons.h>
#include <run.h>
#include <stdlib.h>

int n;                 // Definition for global variable 'n'
/* Now global variable n is accessible by all the processes i.e. consume and produce */

sid32 r_sem_id;
sid32 w_sem_id;
sid32 processes_sem_id;

shellcmd xsh_prodcons(int nargs, char *args[]) {
	// Argument verifications and validations
	int count = 200;    // local varible to hold count
		
	// TODO: check args[1], if present assign value to count
	
	// Check num args == 1 || == 2
	if (!(nargs == 1 || nargs == 2)) {
		printf("prodcons takes 0 or 1 arguements:\n\tprodcons [Integer]\n");
		return 0;
	}
	
	// Check if arg[1] is an int and is > 0 
	if (nargs == 2) {
		if (atoi(args[1]) < 0) {
			printf("Input must be an integer greater or equal to 0!\n");
			return 0;
		}

		count = atoi(args[1]);
	}

	r_sem_id = semcreate(1);
	w_sem_id = semcreate(0);
	processes_sem_id = semcreate(0);

	// create the process producer and consumer and put them in ready queue
	int pid_producer = create(producer, 1024, 1, "producer", 1, count);
	int pid_consumer = create(consumer, 1024, 1, "consumer", 1, count);

	resume(pid_producer);
	resume(pid_consumer);


	// Look at the definations of function create and resume in the system folder for reference.
	// TODO: call the 'resume' and 'create' functions to start two processes that execute
	//       your 'producer' and 'consumer' code.
	wait(processes_sem_id);
	signal(run_sem_id);
	return 0;
}
