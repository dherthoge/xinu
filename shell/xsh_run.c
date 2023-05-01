#include <ctype.h>
//#include <future.h>
#include <future_prodcons.h>
#include <xinu.h>
#include <shprototypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <prodcons_bb.h>

// for prodcons_bb
sid32 write_sem_id, read_sem_id, prodcons_bb_sem_id;
int32 arr_q[5];
int write_idx, read_idx;
sid32 run_sem_id;



//taken from https://stackoverflow.com/questions/29248585/c-checking-command-line-argument-is-integer-or-not
bool isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

sid32 print_sem;
void future_prodcons(int nargs, char *args[]) {
if (nargs < 2) {
       printf("%s\n", "Syntax: run futest [-pc [g ...] [s VALUE ...]|-f]");
       signal(run_sem_id);
       return;
}
  print_sem = semcreate(1);
  future_t* f_exclusive;
  f_exclusive = future_alloc(FUTURE_EXCLUSIVE, sizeof(int), 1);

  // First, try to iterate through the arguments and make sure they are all valid based on the requirements
  // (you should not assume that the argument after "s" is always a number)
  int i = 1;
  char g[] = "g";
  while (i < nargs) {
		printf(args[i]);
		if (strcmp(args[i], g) != 0 && !isNumber(args[i])) {
			printf("%s\n", "Syntax: run futest [-pc [g ...] [s VALUE ...]|-f]");
			future_free(f_exclusive);
       signal(run_sem_id);
			return;
		}
		i++;
  }
/*
      resume(create(future_prod, 2048, 20, "fprod1", 2, f_exclusive, 79));
      resume(create(future_cons, 2048, 20, "cons", 1, f_exclusive));
	future_free(f_exclusive);
*/
  int num_args = i;  // keeping number of args to create the array
  i = 1; // reseting the index

  // Iterate again through the arguments and create the following processes based on the passed argument ("g" or "VALUE")
  while (i < nargs) {
    if (strcmp(args[i], "g") == 0) {
      char id[10];
      sprintf(id, "fcons%d",i);
      resume(create(future_cons, 2048, 20, id, 1, f_exclusive));
    }
    else {
      uint8 number = atoi(args[i]);
      resume(create(future_prod, 2048, 20, "fprod1", 2, f_exclusive, number));
      sleepms(5);
    }
    i++;
  }
  sleepms(100);
  future_free(f_exclusive);
       signal(run_sem_id);
  }


void prodcons_bb(int nargs, char *args[]) {
	write_sem_id = semcreate(0);
	read_sem_id = semcreate(1);
	write_idx = 0;
	read_idx = 0;
	int producerProcCount = atoi(args[1]);
	int consumerProcCount = atoi(args[2]);
	int producerCount = atoi(args[3]);
	int consumerCount = atoi(args[4]);

        if (producerProcCount * producerCount != consumerProcCount * consumerCount) {
                printf("Iteration Mismatch Error: the number of producer(s) iteration does not match the consumer(s) iteration\n");
		signal(run_sem_id);
		return;
        }
	
	for (int i = 0; i < producerProcCount; i++) {
		char procName[20];
		sprintf(procName, "producer_%d", i);
		resume(create(producer_bb, 1024, 20, procName, 1, producerCount));
	}

	prodcons_bb_sem_id = semcreate(0);
	for (int i = 0; i < consumerProcCount; i++) {
		char procName[20];
		sprintf(procName, "consumer_%d", i);
		resume(create(consumer_bb, 1024, 20, procName, 1, consumerCount));
	}

	for (int i = 0; i < consumerProcCount; i++)
		wait(prodcons_bb_sem_id);
	signal(run_sem_id);
}

shellcmd xsh_run(int nargs, char *args[]) {
        // Check if no arguement was provided
        if ((nargs == 1) || (strncmp(args[1], "list", 4) == 0)) {
                printf("hello\n");
                printf("futest\n");
                printf("list\n");
                printf("prodcons\n");
		printf("prodcons_bb\n");
                return 0;
        }

	run_sem_id = semcreate(0);

        if(strncmp(args[1], "hello", 8) == 0) {
                /* create a process with the function as an entry point. */
                resume(create((void *) xsh_hello, 1024, 20, "xsh_hello", 2, nargs - 1, &(args[1])));
	}

	else if(strncmp(args[1], "futest", 6) == 0) {
                /* create a process with the function as an entry point. */
                resume(create((void *) future_prodcons, 1024, 20, "future_prodcons", 2, nargs - 1, &(args[1])));
	}
	
	else if(strncmp(args[1], "memtest", 7) == 0) {
                /* create a process with the function as an entry point. */
                resume(create((void *) xsh_memtest, 1024, 20, "xsh_memtest", 2, nargs - 1, &(args[1])));
	}
	
	else if(strncmp(args[1], "prodcons_bb", 11) == 0) {
		resume(create((void *) prodcons_bb, 1024, 20, "producer_bb", 2, nargs - 1, &(args[1])));
	}

	else if(strncmp(args[1], "prodcons", 8) == 0) {
                /* create a process with the function as an entry point. */
                resume(create((void *) xsh_prodcons, 4096, 20, "xsh_prodcons", 2, nargs - 1, &(args[1])));
	}

	wait(run_sem_id);	
	return 0;
}
