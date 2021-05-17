/*
	PHILO.C
	An implementation of the dining philosophers problem.
*/

#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

//table of philosophers
struct philoState_t {
	int stateNo;
	sem_t updateSem;
};
const char* STATES[] = {"Thinking", "Waiting", "Eating"};
struct philoState_t philoStates[5] = {0};

//semaphore for the printer thread
sem_t printerSem;

void* simulatePhilosopher(void* rawState) {
	//cast to proper pointer for ease of use
	struct philoState_t* state = rawState;
	
	while (true) {
		//wait for status change
		sem_wait(&state->updateSem);
	}
}

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
				STATES[philoStates[i].stateNo]);
				
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
	
	//spawn five philosopher threads
	pthread_t philosophers[5];
	for (int i = 0; i < 5; i++)
		pthread_create(&philosophers[i], NULL, simulatePhilosopher, &philoStates[i]);
	
	//poll stdin for commands
	while (true) {
		//get input, discard invalid values
		int input = getchar() - '1';
		if (input < 0 || input >= 5) {
			sem_post(&printerSem);
			continue;
		}
		
		//change thinking to waiting; waiting or eating to thinking
		philoStates[input].stateNo = !philoStates[input].stateNo;
		sem_post(&philoStates[input].updateSem);
		sem_post(&printerSem);
	}
}
