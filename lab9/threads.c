#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_PATIENTS_IN_HOSPITAL 3
#define MEDICINE_CAPACITY 6
#define CONSULTATION_MEDICINE_USAGE 3

int waiting_patients = 0;
int medicine_stock = 6;
int total_patients = 0;
int patients_remaining = 0;
int patients_treated = 0;
int remaining_dishes = 0;

pthread_cond_t restaurant_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t doctor_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pharmacist_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t patient_cond = PTHREAD_COND_INITIALIZER;


void print_time_prefix() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("[%02d:%02d:%02d] - ", t->tm_hour, t->tm_min, t->tm_sec);
}

void* chef_thread(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);
        while(patients_treated < total_patients){
            pthread_cond_wait(&restaurant_cond, &mutex);
            printf("Dostarczam obiady...");
        }
    
        pthread_mutex_unlock(&mutex);
        break;
    }
}

void* patient_thread(void* arg) {
    int id = *((int*)arg);
    int treated = 0; //czy obsluzony
    
    while (!treated) {
        int wait_time = rand() % 4 + 2;
        print_time_prefix(); 
        printf("Pacjent(%d): Ide do szpitala, bede za %d s\n", id, wait_time);
        sleep(wait_time);

        pthread_mutex_lock(&mutex);
        
        if (waiting_patients >= MAX_PATIENTS_IN_HOSPITAL) {
            int walk_time = rand() % 4 + 2;
            print_time_prefix(); 
            printf("Pacjent(%d): za dużo pacjentów, wracam później za %d s\n", id, walk_time);
            pthread_mutex_unlock(&mutex);
            sleep(walk_time);
            continue;
        }

        waiting_patients++;
        print_time_prefix(); 
        printf("Pacjent(%d): czeka %d pacjentów na lekarza\n", id, waiting_patients);

        if (waiting_patients == 3) {
            print_time_prefix(); 
            printf("Pacjent(%d): budzę lekarza\n", id);
            pthread_cond_signal(&doctor_cond); //budzenie lekarza
        }
        sleep(1);

        while (waiting_patients > 0 && !treated) {
            pthread_cond_wait(&patient_cond, &mutex);
            //czeka na sygnal patent_cond od lekarza
            
            if (waiting_patients == 0 || patients_treated > 0) {
                treated = 1; // Pacjent został obsłużony
                print_time_prefix(); 
                printf("Pacjent(%d): kończę wizytę\n", id);
                break;
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void* pharmacist_thread(void* arg) {
    int id = *((int*)arg);

    while (1) {
        //sprawdzenie czy gotowe
        pthread_mutex_lock(&mutex);
        if (patients_treated >= total_patients) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        
        int wait_time = rand() % 11 + 5;
        print_time_prefix(); 
        printf("Farmaceuta(%d): ide do szpitala, bede za %d s\n", id, wait_time);
        sleep(wait_time);

        pthread_mutex_lock(&mutex);
        
        //sprawdzenie jeszcze raz czy gotowe
        if (patients_treated >= total_patients) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        //jesli pelna to czekac
        if (medicine_stock == MEDICINE_CAPACITY) {
            print_time_prefix(); 
            printf("Farmaceuta(%d): czekam na oproznienie apteczki\n", id);
            pthread_mutex_unlock(&mutex);
            sleep(2);
            continue;
        }

        //jesli ponizej 3 to budzi
        if (medicine_stock < 3) {
            print_time_prefix(); 
            printf("Farmaceuta(%d): budzę lekarza\n", id);
            pthread_cond_signal(&doctor_cond);
        }

        // dostarcza max lekow
        print_time_prefix(); 
        printf("Farmaceuta(%d): dostarczam leki\n", id);
        medicine_stock = MEDICINE_CAPACITY;
        pthread_mutex_unlock(&mutex);
        break;
    }

    pthread_exit(NULL);
}

void* doctor_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        //sprawdzenie czy gotowe
        if (patients_treated >= total_patients) {
            print_time_prefix(); 
            printf("Lekarz: kończę pracę, wszyscy pacjenci zostali obsłużeni\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        if (remaining_dishes < 3 ) {
            print_time_prefix();
            printf("Lekarz przyjmuje dostawę obiadow\n");
            pthread_mutex_unlock(&mutex);

            int delivery_time = rand() % 3 + 1;
            sleep(delivery_time);

            pthread_mutex_lock(&mutex);
            remaining_dishes = 5;
            pthread_cond_broadcast(&restaurant_cond);
            pthread_mutex_unlock(&mutex);
        }

        //sprawdzenie czy sa pacjenci i leki
        if (waiting_patients >= 3 && medicine_stock >= 3) {
            print_time_prefix(); 
            printf("Lekarz: konsultuję pacjentów\n");
            
            waiting_patients -= 3;
            medicine_stock -= 3;
            patients_treated += 3;
            if( remaining_dishes >= 3)
                remaining_dishes -= 3;
            else
                remaining_dishes = 0;
            pthread_cond_broadcast(&patient_cond);

            pthread_mutex_unlock(&mutex);
            //powiadomienie o koncu

            int consult_time = rand() % 3 + 2;
            sleep(consult_time);
            pthread_mutex_lock(&mutex);
            print_time_prefix(); 
            printf("Lekarz: zakończyłem konsultację\n");
            pthread_mutex_unlock(&mutex);
            
        } else if (medicine_stock < 3 && medicine_stock < MEDICINE_CAPACITY) {
            // przyjecie lekow
            print_time_prefix(); 
            printf("Lekarz: przyjmuję dostawę leków\n");
            pthread_mutex_unlock(&mutex);
            
            int delivery_time = rand() % 3 + 1;
            sleep(delivery_time);
            
            pthread_mutex_lock(&mutex);
            medicine_stock = MEDICINE_CAPACITY;
            pthread_cond_broadcast(&pharmacist_cond);
            pthread_mutex_unlock(&mutex);
            
        } else {
            // spanie
            print_time_prefix(); 
            printf("Lekarz: zasypiam\n");
            
            // czekanie na pacjentow/leki
            while ((waiting_patients < 3 || medicine_stock < 3) && patients_treated < total_patients) {
                pthread_cond_wait(&doctor_cond, &mutex);
                print_time_prefix(); 
                printf("Lekarz: budzę się\n");
                
                //wyjscie jak wszyscy gotow
                if (patients_treated >= total_patients) {
                    pthread_mutex_unlock(&mutex);
                    return NULL;
                }
            }
            
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("bledne uzycie funkcji!\n");
        return 1;
    }

    srand(time(NULL));

    int patient_count = atoi(argv[1]);
    int pharmacist_count = atoi(argv[2]);
    total_patients = patient_count;
    patients_remaining = patient_count;

    pthread_t doctor;
    pthread_t* patients = malloc(sizeof(pthread_t) * patient_count);
    pthread_t* pharmacists = malloc(sizeof(pthread_t) * pharmacist_count);
    pthread_t restaurant;

    int* patient_ids = malloc(sizeof(int) * patient_count);
    int* pharmacist_ids = malloc(sizeof(int) * pharmacist_count);

    // lekarz
    pthread_create(&doctor, NULL, doctor_thread, NULL);
    pthread_create(&restaurant, NULL, chef_thread, NULL);

    // pacjenci
    for (int i = 0; i < patient_count; i++) {
        patient_ids[i] = i + 1;
        pthread_create(&patients[i], NULL, patient_thread, &patient_ids[i]);
    }

    // farmaceuci
    for (int i = 0; i < pharmacist_count; i++) {
        pharmacist_ids[i] = i + 1;
        pthread_create(&pharmacists[i], NULL, pharmacist_thread, &pharmacist_ids[i]);
    }

    // czekani na koniec pacjentow
    for (int i = 0; i < patient_count; i++) {
        pthread_join(patients[i], NULL);
        //print_time_prefix(); 
        //printf("Pacjent(%d): kończę wizytę\n", i + 1);
    }

    //czekanie na koniec lekarza
    pthread_join(doctor, NULL);

    pthread_join(restaurant, NULL);

    // czekanie na koniec farmaceutow
    for (int i = 0; i < pharmacist_count; i++) {
        pthread_join(pharmacists[i], NULL);
        print_time_prefix(); 
        printf("Farmaceuta(%d): zakończyłem dostawę\n", i + 1);
    }

    //zwalnianie pamieci
    free(patients);
    free(pharmacists);
    free(patient_ids);
    free(pharmacist_ids);
    
    return 0;
}