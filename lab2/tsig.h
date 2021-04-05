#ifndef NUM_CHILD
	#define NUM_CHILD 7
#endif

#ifdef WITH_SIGNALS
	void setSignalHandler(int, void (*)(int));
#endif

void childHandler();
void interruptHandler(int);
void terminationHandler(int);
void killChildren();
