/*
	PHILO.C
	An implementation of the dining philosophers problem.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#include <signal.h>

//philosopher structure with individual update semaphore
struct philoState_t {
	int stateNo;
	sem_t updateSem;
};

//table of philosophers and the semaphore set for their forks
struct philoState_t philoStates[5] = {0};
int forks = -1;

void printStatus();
void cleanUp();

void* simulatePhilosopher(void* rawState) {
	//cast to proper pointer for ease of use, remember index
	struct philoState_t* state = rawState;
	int index = state - &philoStates[0];
	
	while (true) {
		//wait for status change
		sem_wait(&state->updateSem);
		
		if (state->stateNo == 1) {
			//print waiting state
			printStatus();
			
			//attempt to eat, pick up forks once they're available
			struct sembuf operations[4];
			for (int i = 0; i < 2; i++) {
				operations[i].sem_num = i ? index : ((index + 1) % 5);
				operations[i].sem_op = 0;
				operations[i].sem_flg = 0;
			}
			for (int i = 2; i < 4; i++) {
				operations[i].sem_num = (i - 2) ? index : ((index + 1) % 5);
				operations[i].sem_op = 1;
				operations[i].sem_flg = 0;
			}
			semop(forks, operations, 4);
			
			//on success, set state to eating and reprint
			state->stateNo = 2;
			printStatus();
		}
		else {
			//put away forks
			struct sembuf operations[2];
			for (int i = 0; i < 2; i++) {
				operations[i].sem_num = i ? index : ((index + 1) % 5);
				operations[i].sem_op = -1;
				operations[i].sem_flg = 0;
			}
			semop(forks, operations, 2);
			printStatus();
		}
	}
}

void grabForks(int philosopher) {
	//set state to waiting and update philosopher
	philoStates[philosopher].stateNo = 1;
	sem_post(&philoStates[philosopher].updateSem);
}
void putAwayForks(int philosopher) {
	//don't disturb waiting philosopher
	if (philoStates[philosopher].stateNo == 1)
		return;
	
	//set state to thinking and update philosopher
	philoStates[philosopher].stateNo = 0;
	sem_post(&philoStates[philosopher].updateSem);
}

int main() {
	//create semaphore set for forks
	forks = semget(IPC_PRIVATE, 5, 0x1C0);
	
	//spawn five philosopher threads
	pthread_t philosophers[5];
	for (int i = 0; i < 5; i++)
		pthread_create(&philosophers[i], NULL, simulatePhilosopher, &philoStates[i]);
	
	//break on keyboard interrupt
	struct sigaction action;
	action.sa_handler = cleanUp;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	
	//print initial status
	printStatus();
	
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
	}
}

void printStatus() {
	//state lookup table
	static const char* STATES[] = {"Thinking", "Waiting", "Eating"};
	static const char* FORK_STATES[] = {"Available", "Taken    "};
	
	//clear screen and print philosophers
	printf("\x1B[H\x1B[J");
	printf("PHILOSOPHER    LEFT FORK    RIGHT FORK    STATUS\n");
	
	for (int i = 0; i < 5; i++)
		printf("%d              %s    %s     %s\n",
			i + 1,
			FORK_STATES[semctl(forks, i, GETVAL)],
			FORK_STATES[semctl(forks, (i + 1) % 5, GETVAL)],
			STATES[philoStates[i].stateNo]);
	
	//print prompt
	printf("\n\nWho shall pick up or put down their forks: ");
	fflush(stdout);
}
void cleanUp(int signal) {
	//remove semaphore set and exit
	semctl(forks, 0, IPC_RMID);
	exit(0);
}
