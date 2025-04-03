#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>

#define N_FUEL_BOMBS 2
#define N_CARS 6
pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int globalFuel = 0;
int charged_cars = N_CARS;
sem_t bombas;


void* car(void* arg) {
    sem_wait(&bombas);
    pthread_mutex_lock(&mutexFuel);
    int fuel_needed = rand() % 50 + 1;

    while (globalFuel < fuel_needed) {
        printf("Car %ld waiting for fuel\n", (long)arg);
        pthread_cond_wait(&condFuel, &mutexFuel);
    }

    globalFuel -= fuel_needed;
    printf("Car %ld getting fuel: %d, remaining fuel: %d\n", (long)arg, fuel_needed, globalFuel);
    charged_cars--;
    pthread_mutex_unlock(&mutexFuel);

    sleep(2); // Simulate time taken to refuel
    sem_post(&bombas);
}

void* fuel_bomb(void* arg) {
    while (1)
    {
        pthread_mutex_lock(&mutexFuel);

        if (charged_cars <= 0) {
            pthread_mutex_unlock(&mutexFuel);
            break;
        }


        int fuel_added = rand() % 50 + 50;
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
    sem_init(&bombas, 0, N_FUEL_BOMBS);
    srand(time(NULL));

    for (int i = 0; i < N_CARS + 1; i++) {
        if (i == N_CARS){
            pthread_create(&threads[i], NULL, &fuel_bomb, NULL);
        }
        else {
            pthread_create(&threads[i], NULL, &car, (void*)(long)i);
        }
        
    }
    
    for (int i = 0; i < N_CARS + 1; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    sem_destroy(&bombas);
    return 0;
}