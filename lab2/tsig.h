#include <signal.h>

#ifndef NUM_CHILD
	#define NUM_CHILD 7
#endif

#ifdef WITH_SIGNALS
	void setSignalHandler(int, void (*)(int), struct sigaction* old);
#endif

void childHandler();
void interruptHandler(int);
void terminationHandler(int);
