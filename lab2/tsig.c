#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NUM_CHILD
	#define NUM_CHILD 7
#endif

void setSignalHandler(int, void (*)(int));
void interruptHandler(int);
void terminationHandler(int);

pid_t childPids[NUM_CHILD];

int main() {
	#ifdef WITH_SIGNALS
		//ignore all signals
		for (int signal = 1; signal < 32; signal++)
			setSignalHandler(signal, SIG_IGN);
		
		//restore default child handler, set custom keyboard interrupt handler
		setSignalHandler(SIGCHLD, SIG_DFL);
		setSignalHandler(SIGINT, interruptHandler);
	#endif
	
	//fork the defined number of processes
	for (int i = 0; i < NUM_CHILD; i++) {
		//fork and get pid of child
		pid_t childPid = fork();
		childPids[i] = childPid;
		
		//algorithm for the child process
		if (childPid == 0) {
			#ifdef WITH_SIGNALS
				//disable keyboard interrupt handler, set custom termination handler
				setSignalHandler(SIGINT, SIG_IGN);
				setSignalHandler(SIGTERM, terminationHandler);
			#endif
			
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

void interruptHandler(int signal) {
	printf("Keyboard interrupt received\n");
}
void terminationHandler(int signal) {
	printf("Child process %d terminated\n", getpid());
	exit(0);
}

void setSignalHandler(int signal, void (*handler)(int)) {
	//construct signal action structure
	struct sigaction action;
	action.sa_handler = handler;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	
	//apply action
	sigaction(signal, &action, NULL);
}
