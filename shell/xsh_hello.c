/* xsh_date.c - xsh_date */

#include <xinu.h>
#include <run.h>
#include <fs.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_date - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int nargs, char *args[]) {

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays \"Hello <string arguement>, Welcome to the world of Xinu!!\"\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}

	if (nargs < 1) {
		fprintf(stderr, "You must include a string as the second arguement");
	}
	printf("Hello %s, Welcome to the world of Xinu!!\n", args[1]);
	signal(run_sem_id);
	fs_print_oft();
	return 0;
}
