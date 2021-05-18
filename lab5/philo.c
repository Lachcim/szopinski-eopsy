/*
	PHILO.C
	An implementation of the dining philosophers problem.
*/

#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

//philosopher structure with individual update semaphore
struct philoState_t {
	int stateNo;
	sem_t updateSem;
};

//table of philosophers and the semaphore set for their forks
struct philoState_t philoStates[5] = {0};
int forks = -1;

void* simulatePhilosopher(void* rawState) {
	//cast to proper pointer for ease of use
	struct philoState_t* state = rawState;
	
	while (true) {
		//wait for status change
		sem_wait(&state->updateSem);
	}
}

void grabForks(int philosopher) {
	//set state to waiting and update philosopher
	philoStates[philosopher].stateNo = 1;
	sem_post(&philoStates[philosopher].updateSem);
}
void putAwayForks(int philosopher) {
	//set state to thinking and update philosopher
	philoStates[philosopher].stateNo = 0;
	sem_post(&philoStates[philosopher].updateSem);
}

void printStatus() {
	//state lookup table
	static const char* STATES[] = {"Thinking", "Waiting", "Eating"};
	
	//clear screen and print philosophers
	printf("\x1B[H\x1B[J");
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
int main() {
	//print initial status
	printStatus();
	
	//spawn five philosopher threads
	pthread_t philosophers[5];
	for (int i = 0; i < 5; i++)
		pthread_create(&philosophers[i], NULL, simulatePhilosopher, &philoStates[i]);
	
	//poll stdin for commands
	while (true) {
		//get input, discard invalid values
		int input = getchar() - '1';
		if (input < 0 || input >= 5) {
			printStatus();
			continue;
		}
		
		//grab or put away forks
		if (philoStates[input].stateNo == 0) grabForks(input);
		else putAwayForks(input);
		
		//print immediate changes
		printStatus();
	}
}
