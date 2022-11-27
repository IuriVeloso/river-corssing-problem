#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_BOAT_VISITORS 24

#define ARRIVED_HACKER 0
#define ARRIVED_SERF 1

int passengersOnBoard = 0;
int hackersOnBoard = 0;
int serfsOnBoard = 0;
int totalPassengers = 0;

time_t t;

pthread_cond_t boardOpen = PTHREAD_COND_INITIALIZER;
pthread_cond_t boarding = PTHREAD_COND_INITIALIZER;
pthread_cond_t boardFull = PTHREAD_COND_INITIALIZER;

int isBoardOpen = 0;
int isBoarding = 0;
int isBoardFull = 0;


pthread_mutex_t mutexCountPassengers = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutexCountHackers = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutexCountSerfs = PTHREAD_MUTEX_INITIALIZER;


pthread_mutex_t mutexBoardFull = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCanBoard = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSomeoneBoarding = PTHREAD_MUTEX_INITIALIZER;


void *serf(void *args) {
    printf("\n1- Serf aguarda para embarcar %d!\n", *(int *)args); 

    pthread_mutex_lock(&mutexSomeoneBoarding);
    while(isBoarding == 1){
        pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
        printf("\n1.25- Serf vai verificar %d!\n", *(int *)args); 

        pthread_mutex_lock(&mutexCountPassengers);
        if((serfsOnBoard==0 && hackersOnBoard ==3) || (hackersOnBoard == 1 && serfsOnBoard ==2)){
            printf("\n1.5- Serf nao pode embarcar %d, %d! (h: %d, s:%d, t: %d)\n", *(int *)args, isBoarding, hackersOnBoard, serfsOnBoard, totalPassengers);

            isBoarding = 1;
            pthread_cond_signal(&boarding);

            printf("\n1.75- Serf deveria ter mandado um signal\n");
            pthread_mutex_unlock(&mutexSomeoneBoarding);

        } else {
            isBoarding = 0;
            pthread_mutex_unlock(&mutexSomeoneBoarding);
        }
        pthread_mutex_unlock(&mutexCountPassengers);
        printf("\n1.85- Serf vai verificar %d!\n", *(int *)args); 

    }
    isBoarding = 1;
    printf("\n1.95- Serf vai verificar %d!\n", *(int *)args); 
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n2- Serf na fila para embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexCanBoard);
    while(isBoardOpen == 0){
        pthread_cond_wait(&boardOpen, &mutexCanBoard);
    }
    pthread_mutex_unlock(&mutexCanBoard);

    printf("\n2.5- Serf indo embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexBoardFull);
    while(isBoardFull == 1){
        pthread_cond_wait(&boardFull, &mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexBoardFull);
    
    printf("\n3- Serf indo %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexCountPassengers);
    passengersOnBoard ++;
    serfsOnBoard++;
    if(passengersOnBoard == 4 || totalPassengers + passengersOnBoard == MAX_BOAT_VISITORS){
        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 1;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexCountPassengers);

    printf("\n4- Serf vai liberar pra entrarem %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexSomeoneBoarding);
    isBoarding = 0;
    pthread_cond_signal(&boarding);
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n5- Serf liberou %d!\n", *(int *)args);

    pthread_exit(NULL);
}

void *hacker(void *args) {
    printf("\n1 -Hacker aguarda para embarcar %d!\n", *(int *)args);

    // pthread_mutex_lock(&mutexCountPassengers);
    // while((serfsOnBoard==3 && hackersOnBoard ==0) || (hackersOnBoard == 2 && serfsOnBoard ==1)){
    //     printf("\n1.5- Hacker nao pode embarcar %d! (h: %d, s:%d, t: %d)\n", *(int *)args, hackersOnBoard, serfsOnBoard, totalPassengers);
    //     if(totalPassengers + serfsOnBoard + hackersOnBoard == MAX_BOAT_VISITORS - 1){
    //         pthread_mutex_unlock(&mutexCountPassengers);
    //         pthread_mutex_lock(&mutexBoardFull);
    //         isBoardFull = 1;
    //         pthread_cond_signal(&boardFull);
    //         pthread_mutex_unlock(&mutexBoardFull);
    //         break;
    //     } else {
    //         pthread_mutex_unlock(&mutexCountPassengers);

    //         pthread_mutex_lock(&mutexSomeoneBoarding);
    //         pthread_cond_signal(&boarding);
    //         pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
    //         pthread_mutex_unlock(&mutexSomeoneBoarding);
    //     }
    // }
    // pthread_mutex_unlock(&mutexCountPassengers);  

    pthread_mutex_lock(&mutexSomeoneBoarding);
    while(isBoarding == 1){
        pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
        printf("\n1.25- Hacker vai verificar %d!\n", *(int *)args); 

        pthread_mutex_lock(&mutexCountPassengers);
        if((serfsOnBoard==3 && hackersOnBoard ==0) || (hackersOnBoard == 2 && serfsOnBoard ==1)){
            printf("\n1.5- Hacker nao pode embarcar %d! (h: %d, s:%d, t: %d)\n", *(int *)args, hackersOnBoard, serfsOnBoard, totalPassengers);            
            isBoarding = 1;
            pthread_cond_signal(&boarding);

            printf("\n1.75- Hacker deveria ter mandado um signal\n");
            pthread_mutex_unlock(&mutexSomeoneBoarding);
        }   else {
            isBoarding = 0;
            pthread_mutex_unlock(&mutexSomeoneBoarding);
        }
        printf("\n1.8- Hacker vai verificar %d!\n", *(int *)args); 
        pthread_mutex_unlock(&mutexCountPassengers);
        printf("\n1.9- Hacker vai verificar %d!\n", *(int *)args); 
    }
    printf("\n1.95- Hacker vai verificar %d!\n", *(int *)args); 
    isBoarding = 1;
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n2- Hacker na fila para embarcar %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexCanBoard);
    while(isBoardOpen == 0){
        pthread_cond_wait(&boardOpen, &mutexCanBoard);
    }
    pthread_mutex_unlock(&mutexCanBoard);

    printf("\n2.5- Hacker indo embarcar %d!\n", *(int *)args); 

    pthread_mutex_lock(&mutexBoardFull);
    while(isBoardFull == 1){
        pthread_cond_wait(&boardFull, &mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexBoardFull);  
    
    printf("\n3- Hacker foi %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexCountPassengers);
    passengersOnBoard ++;
    hackersOnBoard++;
    if(passengersOnBoard == 4 || totalPassengers + passengersOnBoard == MAX_BOAT_VISITORS){
        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 1;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexCountPassengers);

    printf("\n4- Hacker vai liberar o mutex %d!\n", *(int *)args);

    pthread_mutex_lock(&mutexSomeoneBoarding);
    isBoarding = 0;
    pthread_cond_signal(&boarding);
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n5- Hacker liberou %d!\n", *(int *)args);

    pthread_exit(NULL);
}

void *barco(void *args) {
    printf("\n\nIniciando as viagens do dia!\n\n");

    while(totalPassengers < MAX_BOAT_VISITORS){
        printf("\n\nEnchendo um novo barco %d\n\n", totalPassengers);

        pthread_mutex_lock(&mutexCountPassengers);
        passengersOnBoard = 0;
        serfsOnBoard = 0;
        hackersOnBoard = 0;
        pthread_mutex_unlock(&mutexCountPassengers);

        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 0;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);

        pthread_mutex_lock(&mutexCanBoard);
        isBoardOpen = 1;
        pthread_cond_signal(&boardOpen);
        pthread_mutex_unlock(&mutexCanBoard);

        
        pthread_mutex_lock(&mutexBoardFull);
        while(isBoardFull == 0){
            pthread_cond_wait(&boardFull, &mutexBoardFull);
        }
        pthread_mutex_unlock(&mutexBoardFull);
 
        pthread_mutex_lock(&mutexCanBoard);
        isBoardOpen = 0;
        pthread_mutex_unlock(&mutexCanBoard);

        printf("\n Barco vai partir! (h: %d, s:%d, p: %d)\n", hackersOnBoard, serfsOnBoard, passengersOnBoard);

        totalPassengers += passengersOnBoard;
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
         } else if (passengerArrival == ARRIVED_HACKER) {
            pthread_create(&threadHackers[i], NULL, hacker, &idsClientes[i]);
         } else {
            printf("Não foi possível identificar quem chegou. (Seed problem)\n");
         }

    }

    pthread_join(threadBarco,NULL);

    printf("\n\nAcabram as viagens de hoje! Volte amanhã para mais!\n\n");
    return 0;
}