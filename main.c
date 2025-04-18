#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>

// VARIÁVEIS GLOBAIS
pthread_mutex_t mutexFuel;
pthread_mutex_t mutexPump;
pthread_cond_t condFuel;
int globalFuel = 0; // combustível suficiente
int unfueled_cars_count;
int N_FUEL_BOMBS;
int N_CARS;
sem_t pumps;
int* bomb_status = NULL;

int occupy_pump(long car_id) {
    int bomb_index = -1;
    pthread_mutex_lock(&mutexPump);
    for (int i = 0; i < N_FUEL_BOMBS; i++) {
        if (bomb_status[i] == -1) {
            bomb_status[i] = car_id;
            bomb_index = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutexPump);
    return bomb_index;
}

void release_pump(int pump_index) {
    pthread_mutex_lock(&mutexPump);
    bomb_status[pump_index] = -1;
    pthread_mutex_unlock(&mutexPump);
}

void* car(void* arg) {
    long id = (long)arg;
    sem_wait(&pumps);  // Aguarda bomba
    int my_pump_index = occupy_pump(id);

    pthread_mutex_lock(&mutexFuel);
    int fuel_needed = rand() % 50 + 1;

    while (globalFuel < fuel_needed) {
        printf("Car %ld waiting for fuel\n", (long)arg);
        pthread_cond_wait(&condFuel, &mutexFuel);
    }

    globalFuel -= fuel_needed;
    printf("⛽ Car %ld is fueling at pump %d → Got %dL | Remaining: %dL\n", id, my_pump_index, fuel_needed, globalFuel);
    unfueled_cars_count--;

    pthread_mutex_unlock(&mutexFuel);
    sleep(1);  // tempo proporcional
    release_pump(my_pump_index);  // Libera bomba
    sem_post(&pumps);  // Libera bomba
    return NULL;
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
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <number_of_cars> <number_of_fuel_pumps>\n", argv[0]);
        return 1;
    }

    // Lê os parâmetros passados pela linha de comando
    N_CARS = atoi(argv[1]);
    N_FUEL_BOMBS = atoi(argv[2]);

    if (N_CARS <= 0 || N_FUEL_BOMBS <= 0) {
        printf("Error: number_of_cars and number_of_fuel_pumps must be greater than 0.\n");
        return 1;
    }

    // Configurações iniciais
    pthread_t threads[N_CARS + 1];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_mutex_init(&mutexPump, NULL);
    pthread_cond_init(&condFuel, NULL);
    sem_init(&pumps, 0, N_FUEL_BOMBS);

    bomb_status = malloc(sizeof(int) * N_FUEL_BOMBS);
    for (int i = 0; i < N_FUEL_BOMBS; i++) bomb_status[i] = -1;

    unfueled_cars_count = N_CARS;

    srand(time(NULL));

    // Criação das threads (carros)
    for (int i = 0; i <= N_CARS; i++) {
        if (i != N_CARS){
            pthread_create(&threads[i], NULL, &car, (void*)(long)i);    
        }
        else {
            pthread_create(&threads[i], NULL, &fuel_bomb, NULL);
        }
        
    }

    // Espera as threads terminarem
    for (int i = 0; i <= N_CARS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Liberação de recursos
    free(bomb_status);
    pthread_mutex_destroy(&mutexFuel);
    pthread_mutex_destroy(&mutexPump);
    pthread_cond_destroy(&condFuel);
    sem_destroy(&pumps);

    printf("\n🚦 Todos os carros abastecidos. Fim.\n");
    return 0;
}
