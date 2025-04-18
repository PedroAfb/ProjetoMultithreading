#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>

#define N_FUEL_BOMBS 100
#define N_CARS 300
pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int globalFuel = 0;
int unfueled_cars_count = N_CARS;
sem_t pumps;


void* car(void* arg) {
    sem_wait(&pumps);
    pthread_mutex_lock(&mutexFuel);
    int fuel_needed = rand() % 50 + 1;

    while (globalFuel < fuel_needed) {
        printf("Car %ld waiting for fuel\n", (long)arg);
        pthread_cond_wait(&condFuel, &mutexFuel);
    }

    globalFuel -= fuel_needed;
    printf("Car %ld getting fuel: %d, remaining fuel: %d\n", (long)arg, fuel_needed, globalFuel);
    unfueled_cars_count--;
    pthread_mutex_unlock(&mutexFuel);

    sleep(2); // Simulate time taken to refuel
    sem_post(&pumps);
}

void* fuel_bomb(void* arg) {
    while (1)
    {
        pthread_mutex_lock(&mutexFuel);

        if (unfueled_cars_count <= 0) {
            pthread_mutex_unlock(&mutexFuel);
            break;
        }


        int fuel_added = N_FUEL_BOMBS*50;
        globalFuel += fuel_added;
        printf("Fuel bomb adding %d fuel ...\nNew total fuel: %d\n", fuel_added, globalFuel);

        pthread_cond_broadcast(&condFuel);
        pthread_mutex_unlock(&mutexFuel);

        sleep(5); // Simulate time to add fuel again

    }
}

int main(){
    pthread_t threads[N_CARS + 1];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);
    sem_init(&pumps, 0, N_FUEL_BOMBS);
    srand(time(NULL));

    for (int i = 0; i <= N_CARS; i++) {
        if (i != N_CARS){
            pthread_create(&threads[i], NULL, &car, (void*)(long)i);
        }
        else {
            pthread_create(&threads[i], NULL, &fuel_bomb, NULL);
        }
        
    }
    
    for (int i = 0; i < N_CARS + 1; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    sem_destroy(&pumps);
    return 0;
}