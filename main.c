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

#include <sys/time.h>

void log_with_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *t = localtime(&tv.tv_sec);

    printf("[%02d:%02d:%02d.%03ld] ", 
        t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);
		fflush(stdout);
}

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
		log_with_timestamp();
		printf("⛽ Car %ld is fueling at pump %d\n", id, my_pump_index);
		sleep(10+my_pump_index%4*5);

    pthread_mutex_lock(&mutexFuel);
    int fuel_needed = rand() % 50 + 1;

    while (globalFuel < fuel_needed) {
			log_with_timestamp();
        printf("Car %ld waiting for fuel\n", (long)arg);
        pthread_cond_wait(&condFuel, &mutexFuel);
    }

    globalFuel -= fuel_needed;
		

    unfueled_cars_count--;

    pthread_mutex_unlock(&mutexFuel);
    sleep(fuel_needed);  // tempo proporcional	
		log_with_timestamp();
    printf("⛽ Car %ld -> Got %dL | Remaining: %dL\n", id, fuel_needed, globalFuel);
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

				log_with_timestamp();
        printf("Fuel bomb adding %d fuel ...\nNew total fuel: %d\n", fuel_added, globalFuel);

        pthread_cond_broadcast(&condFuel);
        pthread_mutex_unlock(&mutexFuel);

        sleep(5); // Simulate time to add fuel again

    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
				
				log_with_timestamp();
        printf("Usage: %s <number_of_cars> <number_of_fuel_pumps>\n", argv[0]);
        return 1;
    }

    // Lê os parâmetros passados pela linha de comando
    N_CARS = atoi(argv[1]);
    N_FUEL_BOMBS = atoi(argv[2]);

    if (N_CARS <= 0 || N_FUEL_BOMBS <= 0) {
				
				log_with_timestamp();
        printf("Error: number_of_cars and number_of_fuel_pumps must be greater than 0.\n");
        return 1;
    }
		
		log_with_timestamp();
		printf("Number of cars: %d\n", N_CARS);

		log_with_timestamp();
		printf("Number of pumps: %d\n", N_FUEL_BOMBS);

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
		
		log_with_timestamp();
    printf("\n🚦 Todos os carros abastecidos. Fim.\n");
    return 0;
}
