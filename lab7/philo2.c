/*
	PHILO.C
	An implementation of the dining philosophers problem.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

//define philosopher states
#define THINKING	0
#define HUNGRY		1
#define EATING		2

//declare mutexes and philosopher states
pthread_mutex_t grabMutex;
pthread_mutex_t philoMutexes[5];
int philoStates[5] = {0};

void grabForks(int);
void putAwayForks(int);
void tryEat(int);

void* simulatePhilosopher(void* rawIndex) {
	//get philosopher index
	int index = (int*)rawIndex - (int*)&philoStates;
	
	//simulate dining and eating
	printf("Philosopher %d grabbing fork\n", index);
	grabForks(index);
	printf("Philosopher %d eating\n", index);
	sleep(2);
	printf("Philosopher %d putting fork away\n", index);
	putAwayForks(index);
}

void grabForks(int index) {
	//ensure atomicity when grabbing forks
	pthread_mutex_lock(&grabMutex);
	
	//cease thinking, become hungry
	philoStates[index] = HUNGRY;
	tryEat(index);
	
	pthread_mutex_unlock(&grabMutex);
	
	//wait for forks to become available, reset lock
	pthread_mutex_lock(&philoMutexes[index]);
}
void putAwayForks(int index) {
	//ensure atomicity when putting forks away
	pthread_mutex_lock(&grabMutex);
	
	//cease eating, start thinking
	philoStates[index] = THINKING;
	
	//let neighbors eat if they're hungry
	tryEat((index + 4) % 5);
	tryEat((index + 1) % 5);
	
	pthread_mutex_unlock(&grabMutex);
}
void tryEat(int index) {
	//don't force feed thinking philosophers
	if (philoStates[index] != HUNGRY)
		return;
	
	if (philoStates[(index + 4) % 5] != EATING &&
		philoStates[(index + 1) % 5] != EATING) {
		
		//mark philosopher as eating, signal fork availability
		philoStates[index] = EATING;
		pthread_mutex_unlock(&philoMutexes[index]);
	}
}

int main() {
	//initialize grab mutex
	pthread_mutex_init(&grabMutex, NULL);
	
	//initialize philosopher mutexes
	for (int i = 0; i < 5; i++) {
		pthread_mutex_init(&philoMutexes[i], NULL);
		pthread_mutex_lock(&philoMutexes[i]);
	}
	
	//spawn five philosopher threads
	pthread_t philosophers[5];
	for (int i = 0; i < 5; i++) {
		pthread_create(&philosophers[i], NULL, simulatePhilosopher, &philoStates[i]);
	}
	
	//wait for them to finish
	for (int i = 0; i < 5; i++)
		pthread_join(philosophers[i], NULL);
}
