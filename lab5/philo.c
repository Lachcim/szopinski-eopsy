/*
	PHILO.C
	An implementation of the dining philosophers problem.
*/

#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

//table of philosophers
const char* STATES[] = {"Thinking", "Eating", "Waiting"};
int philoStates[5] = {0};

//semaphore for the printer thread
sem_t printerSem;

void* printStatus(void* arg) {
	while (true) {
		//wait for status change
		sem_wait(&printerSem);
		
		//clear screen
		printf("\x1B[H\x1B[J");
		
		//print philosophers
		printf("PHILOSOPHER    LEFT FORK    RIGHT FORK    STATUS\n");
		for (int i = 0; i < 5; i++)
			printf("%d              %d            %d             %s\n",
				i + 1,
				1,
				1,
				STATES[philoStates[i]]);
				
		//print prompt
		printf("\n\nWho shall pick up or put down their forks: ");
		fflush(stdout);
	}
}

int main() {
	//spawn printer thread - will print on state change
	pthread_t printer;
	pthread_create(&printer, NULL, printStatus, NULL);
	sem_post(&printerSem);
	
	while (true) {
		getchar();
		sem_post(&printerSem);
	}
}
