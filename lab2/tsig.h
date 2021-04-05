#ifndef NUM_CHILD
	#define NUM_CHILD 7
#endif

#ifdef WITH_SIGNALS
	#include <signal.h>
	
	void setSignalHandler(int, void (*)(int), struct sigaction* old);
	void interruptHandler(int);
	void terminationHandler(int);
#endif

void childHandler();
