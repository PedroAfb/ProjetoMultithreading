#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>

// VARIÁVEIS GLOBAIS
pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int globalFuel; // combustível suficiente
int unfueled_cars_count;
sem_t pumps;

void* car(void* arg) {
    long id = (long)arg;
    sem_wait(&pumps);  // Aguarda bomba

    pthread_mutex_lock(&mutexFuel);
    int fuel_needed = rand() % 50 + 1;

    if (globalFuel >= fuel_needed) {
        globalFuel -= fuel_needed;
        printf("✅ Car %ld got %d fuel. Remaining fuel: %d\n", id, fuel_needed, globalFuel);
    } else {
        printf("❌ Car %ld could not get fuel! (Needed: %d, Available: %d)\n", id, fuel_needed, globalFuel);
    }

    pthread_mutex_unlock(&mutexFuel);
    sleep(1);  // tempo proporcional
    sem_post(&pumps);  // Libera bomba
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <number_of_cars> <number_of_fuel_pumps>\n", argv[0]);
        return 1;
    }

    // Lê os parâmetros passados pela linha de comando
    int N_CARS = atoi(argv[1]);
    int N_FUEL_BOMBS = atoi(argv[2]);

    if (N_CARS <= 0 || N_FUEL_BOMBS <= 0) {
        printf("Error: number_of_cars and number_of_fuel_pumps must be greater than 0.\n");
        return 1;
    }

    // Configurações iniciais
    pthread_t threads[N_CARS];
    pthread_mutex_init(&mutexFuel, NULL);
    sem_init(&pumps, 0, N_FUEL_BOMBS);
    
    globalFuel = N_CARS * 100;  // combustível suficiente para todos os carros
    unfueled_cars_count = N_CARS;

    srand(time(NULL));

    // Criação das threads (carros)
    for (int i = 0; i < N_CARS; i++) {
        pthread_create(&threads[i], NULL, &car, (void*)(long)i);
    }

    // Espera as threads terminarem
    for (int i = 0; i < N_CARS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Liberação de recursos
    pthread_mutex_destroy(&mutexFuel);
    sem_destroy(&pumps);

    printf("\n🚦 Todos os carros abastecidos. Fim.\n");
    return 0;
}
