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
		struct sigaction oldActions[NSIG - 1];
		for (int signal = 1; signal < NSIG; signal++)
			setSignalHandler(signal, SIG_IGN, &oldActions[signal - 1]);
		
		//restore default child handler, set custom keyboard interrupt handler
		setSignalHandler(SIGCHLD, SIG_DFL, NULL);
		setSignalHandler(SIGINT, interruptHandler, NULL);
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
			fprintf(stderr, "parent[%d]: child #%d failed to spawn, aborting\n", getpid(), i);
			
			for (int j = 0; j < i; j++)
				kill(childPids[j], SIGTERM);
				
			return 1;
		}
		
		//insert delay between forks
		if (i != NUM_CHILD - 1)
			sleep(1);
		
		//break loop on keyboard interrupt
		if (aborted) {
			fprintf(stderr, "parent[%d]: aborting due to a keyboard interrupt\n", getpid());
			
			for (int j = 0; j <= i; j++)
				kill(childPids[j], SIGTERM);
				
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
	
	printf("parent[%d]: %d child processes terminated\n", getpid(), terminations);
	
	#ifdef WITH_SIGNALS
		//restore old handlers
		for (int signal = 1; signal < NSIG; signal++)
			sigaction(signal, &oldActions[signal - 1], NULL);
	#endif
}

void childHandler() {
	#ifdef WITH_SIGNALS
		//disable keyboard interrupt handler, set custom termination handler
		setSignalHandler(SIGINT, SIG_IGN, NULL);
		setSignalHandler(SIGTERM, terminationHandler, NULL);
	#endif
	
	//live for 10 seconds and print status messages
	printf("child[%d]: new process born, my parent is %d\n", getpid(), getppid());
	sleep(10);
	printf("child[%d]: process finished\n", getpid());
}
void interruptHandler(int signal) {
	//print message and set abort flag
	fprintf(stderr, "parent[%d]: keyboard interrupt received\n", getpid());
	aborted = true;
}
void terminationHandler(int signal) {
	//print message and exit
	fprintf(stderr, "child[%d]: process terminated\n", getpid());
	exit(0);
}

#ifdef WITH_SIGNALS
	void setSignalHandler(int signal, void (*handler)(int), struct sigaction* old) {
		//construct signal action structure
		struct sigaction action;
		action.sa_handler = handler;
		action.sa_flags = 0;
		sigemptyset(&action.sa_mask);
		
		//apply action
		sigaction(signal, &action, old);
	}
#endif
