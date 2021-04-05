#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "tsig.h"

pid_t childPids[NUM_CHILD];
bool aborted = false;

int main() {
	#ifdef WITH_SIGNALS
		//ignore all signals
		for (int signal = 1; signal < NSIG; signal++)
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
			childHandler();
			return 0;
		}
		
		//terminate living children on fork failure
		if (childPid == -1) {
			fprintf(stderr, "Child #%d failed to spawn, aborting\n", i);
			killChildren(i);
			return 1;
		}
		
		//insert delay between forks
		if (i != NUM_CHILD - 1)
			sleep(1);
		
		//break loop on keyboard interrupt
		if (aborted) {
			fprintf(stderr, "Aborting due to a keyboard interrupt\n");
			killChildren(i + 1);
			break;
		}
	}
	
	//wait for children to terminate
	int terminations = 0;
	while (1) {
		//continue on interrupts, break when finished
		if (wait(NULL) == -1) {
			if (errno == EINTR) continue;
			if (errno == ECHILD) break;
		}
		
		terminations++;
	}
	
	printf("%d child processes terminated\n", terminations);
}

void childHandler() {
	#ifdef WITH_SIGNALS
		//disable keyboard interrupt handler, set custom termination handler
		setSignalHandler(SIGINT, SIG_IGN);
		setSignalHandler(SIGTERM, terminationHandler);
	#endif
	
	//live for 10 seconds and print status messages
	printf("Child process %d born, my parent is %d\n", getpid(), getppid());
	sleep(10);
	printf("Child process %d signing out\n", getpid());
}
void interruptHandler(int signal) {
	//print message and set abort flag
	printf("Keyboard interrupt received\n");
	aborted = true;
}
void terminationHandler(int signal) {
	//print message and exit
	printf("Child process %d terminated\n", getpid());
	exit(0);
}
void killChildren(int count) {
	//terminate all child processes
	for (int i = 0; i < count; i++)
		kill(childPids[i], SIGTERM);
}

#ifdef WITH_SIGNALS
	void setSignalHandler(int signal, void (*handler)(int)) {
		//construct signal action structure
		struct sigaction action;
		action.sa_handler = handler;
		action.sa_flags = 0;
		sigemptyset(&action.sa_mask);
		
		//apply action
		sigaction(signal, &action, NULL);
	}
#endif
