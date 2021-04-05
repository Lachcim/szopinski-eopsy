#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>

#ifndef NUM_CHILD
	#define NUM_CHILD 7
#endif

pid_t childPids[NUM_CHILD];

int main() {
	//fork the defined number of processes
	for (int i = 0; i < NUM_CHILD; i++) {
		//fork and get pid of child
		pid_t childPid = fork();
		childPids[i] = childPid;
		
		//algorithm for the child process
		if (childPid == 0) {
			printf("Child process %d born, my parent is %d\n", getpid(), getppid());
			sleep(10);
			
			printf("Child process %d signing out\n", getpid());
			return 0;
		}
		
		//terminate living children on fork failure
		if (childPid == -1) {
			fprintf(stderr, "Child #%d failed to spawn, aborting\n", i);
			
			for (int j = 0; j < i; j++)
				kill(childPids[j], SIGTERM);
			
			return 1;
		}
		
		//insert delay between forks
		if (i != NUM_CHILD - 1)
			sleep(1);
	}
	
	printf("All child processes forked\n");
	
	//wait for children to terminate
	int terminations = 0;
	while (wait(NULL) != -1)
		terminations++;
	
	printf("All %d child processes terminated\n", terminations);
}
