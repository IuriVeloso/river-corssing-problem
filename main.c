#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_BOAT_VISITORS 20

#define ARRIVED_HACKER 0
#define ARRIVED_SERF 1

int passengersOnBoard = 0;
int totalPassengers = 0;

time_t t;

pthread_cond_t boardOpen = PTHREAD_COND_INITIALIZER;
pthread_cond_t boarding = PTHREAD_COND_INITIALIZER;
pthread_cond_t boardFull = PTHREAD_COND_INITIALIZER;

int isBoardOpen = 0;
int isBoarding = 0;
int isBoardFull = 0;

pthread_mutex_t mutexCountPassengers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBoardFull = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutexCanBoard = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSomeoneBoarding = PTHREAD_MUTEX_INITIALIZER;


void *serf(void *args) {
    printf("\n\n1- Serf aguarda para embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexCanBoard);
    while(isBoardOpen == 0){
        pthread_cond_wait(&boardOpen, &mutexCanBoard);
    }
    pthread_mutex_unlock(&mutexCanBoard);

    printf("\n2- Serf na fila para embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexSomeoneBoarding);
    while(isBoarding == 1){
        pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
    }
    isBoarding = 1;
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n3- Serf aguarda seu lugar no barco %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexBoardFull);
    while(isBoardFull == 1){
        pthread_cond_wait(&boardFull, &mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexBoardFull);

    printf("\n4 -Serf esta embarcando %d\n", *(int *)args);
    
    pthread_mutex_lock(&mutexCountPassengers);
    passengersOnBoard ++;
    if(passengersOnBoard == 4){
        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 1;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexCountPassengers);

    printf("\n5 -Vai o serf %d\n\n", *(int *)args);

    pthread_mutex_lock(&mutexSomeoneBoarding);
    isBoarding = 0;
    pthread_cond_signal(&boarding);
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n6 -Liberou a vaga %d\n\n", *(int *)args);

    pthread_exit(NULL);
}

void *hacker(void *args) {
    printf("\n\n1 -Hacker aguarda para embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexCanBoard);
    while(isBoardOpen == 0){
        pthread_cond_wait(&boardOpen, &mutexCanBoard);
    }
    pthread_mutex_unlock(&mutexCanBoard);

    printf("\n2- Hacker na fila para embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexSomeoneBoarding);
    while(isBoarding == 1){
        pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
    }
    isBoarding = 1;
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n3- Hacker aguarda seu lugar no barco %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexBoardFull);
    while(isBoardFull == 1){
        pthread_cond_wait(&boardFull, &mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexBoardFull);

    printf("\n4 -Hacker esta embarcando %d\n", *(int *)args);
    
    pthread_mutex_lock(&mutexCountPassengers);
    passengersOnBoard ++;
    if(passengersOnBoard == 4){
        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 1;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexCountPassengers);

    printf("\n5 -Vai o hacker %d\n\n", *(int *)args);

    pthread_mutex_lock(&mutexSomeoneBoarding);
    isBoarding = 0;
    pthread_cond_signal(&boarding);
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n6 -Liberou a vaga %d\n\n", *(int *)args);

    pthread_exit(NULL);
}

void *barco(void *args) {
    printf("\n\nIniciando as viagens do dia!\n\n");

    while(totalPassengers < MAX_BOAT_VISITORS){
        printf("\n\nEnchendo o barco %d\n\n", totalPassengers);
        
        pthread_mutex_lock(&mutexCanBoard);
        isBoardOpen = 1;
        pthread_cond_signal(&boardOpen);
        pthread_mutex_unlock(&mutexCanBoard);

        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 0;
        passengersOnBoard = 0;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);

        
        pthread_mutex_lock(&mutexBoardFull);
        while(isBoardFull == 0){
            pthread_cond_wait(&boardFull, &mutexBoardFull);
        }
        pthread_mutex_unlock(&mutexBoardFull);

        printf("\n\nPartiu um barco com %d\n\n", passengersOnBoard);
 
        pthread_mutex_lock(&mutexCanBoard);
        isBoardOpen = 0;
        pthread_mutex_unlock(&mutexCanBoard);

        totalPassengers += 4;
    }
    pthread_exit(NULL);
}

int main(void) {
    // Thread para o grupo de Hackers e Serfs que vão cruzar o Rio
    pthread_t threadHackers[MAX_BOAT_VISITORS];
    pthread_t threadSerfs[MAX_BOAT_VISITORS];

    int idsClientes[MAX_BOAT_VISITORS];
    // Thread do barco
    pthread_t threadBarco;

    int passengerArrival;

    srand((unsigned) time(&t));

    pthread_create(&threadBarco, NULL, barco, NULL);

    for(int i =0; i< MAX_BOAT_VISITORS; i++){
         passengerArrival = rand()%2;
         idsClientes[i] = i;
         if (passengerArrival == ARRIVED_SERF){
            pthread_create(&threadSerfs[i], NULL, serf, &idsClientes[i]);
            printf("Chegou um serf %d\n", i);
         } else if (passengerArrival == ARRIVED_HACKER) {
            pthread_create(&threadHackers[i], NULL, hacker, &idsClientes[i]);
            printf("Chegou um hacker %d\n", i);
         } else {
            printf("Não foi possível identificar quem chegou. (Seed problem)\n");
         }

    }

    pthread_join(threadBarco,NULL);

    printf("\n\nAcabram as viagens de hoje! Volte amanhã para mais!\n\n");
    return 0;
}