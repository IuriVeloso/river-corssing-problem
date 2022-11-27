#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Número de passageiros embvarcadndo (threads criadas). Por definição, sempre inserir um múltiplo de 4
#define MAX_BOAT_VISITORS 24

// Variáveis de definição se quem chegou foi um hacker ou um serf
#define ARRIVED_HACKER 0
#define ARRIVED_SERF 1

// Somatório de passagerios a bordo, hackers a bordo, serfs a bordo e total de passageiros transportados
int passengersOnBoard = 0;
int hackersOnBoard = 0;
int serfsOnBoard = 0;
int totalPassengers = 0;

// Timer
time_t t;

// Variáveis de condição

// Condição, mutex e variável do barco estar aceitando passageiros
pthread_cond_t boardOpen = PTHREAD_COND_INITIALIZER;
int isBoardOpen = 0;
pthread_mutex_t mutexCanBoard = PTHREAD_MUTEX_INITIALIZER;

/// Condição de alguém estar embarcando
pthread_cond_t boarding = PTHREAD_COND_INITIALIZER;
int isBoarding = 0;
pthread_mutex_t mutexSomeoneBoarding = PTHREAD_MUTEX_INITIALIZER;

// Condição do barco já estar cheio de passageiros ou não
pthread_cond_t boardFull = PTHREAD_COND_INITIALIZER;
int isBoardFull = 0;
pthread_mutex_t mutexBoardFull = PTHREAD_MUTEX_INITIALIZER;


// Mutex do count de passageiros
pthread_mutex_t mutexCountPassengers = PTHREAD_MUTEX_INITIALIZER;

void *serf(void *args) {
    printf("\n1- Serf aguarda para embarcar %d!\n", *(int *)args); 

    // Check se o serf pode começar a embarcar
    pthread_mutex_lock(&mutexSomeoneBoarding);
    while(isBoarding == 1){
        pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
        // printf("\n1.25- Serf vai verificar %d!\n", *(int *)args); 

        // pthread_mutex_lock(&mutexCountPassengers);
        // if((serfsOnBoard==0 && hackersOnBoard ==3) || (hackersOnBoard == 1 && serfsOnBoard ==2)){
        //     printf("\n1.5- Serf nao pode embarcar %d, %d! (h: %d, s:%d, t: %d)\n", *(int *)args, isBoarding, hackersOnBoard, serfsOnBoard, totalPassengers);

        //     isBoarding = 1;
        //     pthread_cond_signal(&boarding);

        //     printf("\n1.75- Serf deveria ter mandado um signal\n");
        //     pthread_mutex_unlock(&mutexSomeoneBoarding);

        // } else {
        //     isBoarding = 0;
        //     pthread_mutex_unlock(&mutexSomeoneBoarding);
        // }
        // pthread_mutex_unlock(&mutexCountPassengers);
        // printf("\n1.85- Serf vai verificar %d!\n", *(int *)args); 

    }
    isBoarding = 1;
    printf("\n1.95- Serf vai verificar %d!\n", *(int *)args); 
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n2- Serf na fila para embarcar %d!\n", *(int *)args);

    // Check se o barco está com passagens abertas para embarque ou não
    pthread_mutex_lock(&mutexCanBoard);
    while(isBoardOpen == 0){
        pthread_cond_wait(&boardOpen, &mutexCanBoard);
    }
    pthread_mutex_unlock(&mutexCanBoard);

    printf("\n2.5- Serf indo embarcar %d!\n", *(int *)args);

    // Check se o barco está cheio de passageiros
    pthread_mutex_lock(&mutexBoardFull);
    while(isBoardFull == 1){
        pthread_cond_wait(&boardFull, &mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexBoardFull);
    
    printf("\n3- Serf indo %d!\n", *(int *)args);

    // Serf embarca e verificação se o número máximo de passageiros já foi atingido ou não
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

    // Serf libera o mutex do isBoarding para outro passageiro embarcar
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

    // Check se o hacker pode começar a embarcar
    pthread_mutex_lock(&mutexSomeoneBoarding);
    while(isBoarding == 1){
        pthread_cond_wait(&boarding, &mutexSomeoneBoarding);
        // printf("\n1.25- Hacker vai verificar %d!\n", *(int *)args); 

        // pthread_mutex_lock(&mutexCountPassengers);
        // if((serfsOnBoard==3 && hackersOnBoard ==0) || (hackersOnBoard == 2 && serfsOnBoard ==1)){
        //     printf("\n1.5- Hacker nao pode embarcar %d! (h: %d, s:%d, t: %d)\n", *(int *)args, hackersOnBoard, serfsOnBoard, totalPassengers);            
        //     isBoarding = 1;
        //     pthread_cond_signal(&boarding);

        //     printf("\n1.75- Hacker deveria ter mandado um signal\n");
        //     pthread_mutex_unlock(&mutexSomeoneBoarding);
        // }   else {
        //     isBoarding = 0;
        //     pthread_mutex_unlock(&mutexSomeoneBoarding);
        // }
        // printf("\n1.8- Hacker vai verificar %d!\n", *(int *)args); 
        // pthread_mutex_unlock(&mutexCountPassengers);
        // printf("\n1.9- Hacker vai verificar %d!\n", *(int *)args); 
    }
    printf("\n1.95- Hacker vai verificar %d!\n", *(int *)args); 
    isBoarding = 1;
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n2- Hacker na fila para embarcar %d!\n", *(int *)args);

    // Check se o barco está com passagens abertas para embarque ou não
    pthread_mutex_lock(&mutexCanBoard);
    while(isBoardOpen == 0){
        pthread_cond_wait(&boardOpen, &mutexCanBoard);
    }
    pthread_mutex_unlock(&mutexCanBoard);

    printf("\n2.5- Hacker indo embarcar %d!\n", *(int *)args); 

    // Check se o barco já está cheio de passageiros
    pthread_mutex_lock(&mutexBoardFull);
    while(isBoardFull == 1){
        pthread_cond_wait(&boardFull, &mutexBoardFull);
    }
    pthread_mutex_unlock(&mutexBoardFull);  
    
    printf("\n3- Hacker foi %d!\n", *(int *)args);

    // Hacker embarca e verificação se o número máximo de passageiros já foi atingido ou não
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

    // Hacker libera o mutex do isBoarding para outro passageiro embarcar
    pthread_mutex_lock(&mutexSomeoneBoarding);
    isBoarding = 0;
    pthread_cond_signal(&boarding);
    pthread_mutex_unlock(&mutexSomeoneBoarding);

    printf("\n5- Hacker liberou %d!\n", *(int *)args);

    pthread_exit(NULL);
}

void *barco(void *args) {
    printf("\n\nIniciando as viagens do dia!\n\n");
    // Início da thread do barco
    while(totalPassengers < MAX_BOAT_VISITORS){
        printf("\n\nEnchendo um novo barco %d\n\n", totalPassengers);

        // Seta o número de passageiros, tanto a bordo quanto de um tipo ou outro para 0
        pthread_mutex_lock(&mutexCountPassengers);
        passengersOnBoard = 0;
        serfsOnBoard = 0;
        hackersOnBoard = 0;
        pthread_mutex_unlock(&mutexCountPassengers);

        // Seta a condição de estado do barco para vazia
        pthread_mutex_lock(&mutexBoardFull);
        isBoardFull = 0;
        pthread_cond_signal(&boardFull);
        pthread_mutex_unlock(&mutexBoardFull);

        // Anuncia que o embarque começou
        pthread_mutex_lock(&mutexCanBoard);
        isBoardOpen = 1;
        pthread_cond_signal(&boardOpen);
        pthread_mutex_unlock(&mutexCanBoard);

        // Aguarda o barco ficar cheio para zarpar
        pthread_mutex_lock(&mutexBoardFull);
        while(isBoardFull == 0){
            pthread_cond_wait(&boardFull, &mutexBoardFull);
        }
        pthread_mutex_unlock(&mutexBoardFull);
 
        // Anuncia que o barco nçao aceita novos tripulantes
        pthread_mutex_lock(&mutexCanBoard);
        isBoardOpen = 0;
        pthread_mutex_unlock(&mutexCanBoard);

        printf("\n Barco vai partir! (h: %d, s:%d, p: %d)\n", hackersOnBoard, serfsOnBoard, passengersOnBoard);

        // Soma o número de pessoas que embarcou ao total de pessoas transportadas
        totalPassengers += passengersOnBoard;
    }
    pthread_exit(NULL);
}

int main(void) {
    // Thread para o grupo de Hackers e Serfs que vão cruzar o Rio
    pthread_t threadHackers[MAX_BOAT_VISITORS];
    pthread_t threadSerfs[MAX_BOAT_VISITORS];

    // Check de ids dos tripulantes
    int idsClientes[MAX_BOAT_VISITORS];

    // Thread do barco
    pthread_t threadBarco;

    // Check de passageiro que chega no barco
    int passengerArrival;

    // Seed da rand
    srand((unsigned) time(&t));

    // Criando thread de barcos
    pthread_create(&threadBarco, NULL, barco, NULL);

    // Loop para criar numero de threas especificadas pela variável MAX_BOAT_VISITORS, escolhendo aleatoriamente entre Serfs e Hackers
    for(int i =0; i< MAX_BOAT_VISITORS; i++){
         passengerArrival = rand()%2;
         idsClientes[i] = i;
         // Caso em que um serf foir criado
         if (passengerArrival == ARRIVED_SERF){
            pthread_create(&threadSerfs[i], NULL, serf, &idsClientes[i]);
        // Caso em que um hacker foir criado
         } else if (passengerArrival == ARRIVED_HACKER) {
            pthread_create(&threadHackers[i], NULL, hacker, &idsClientes[i]);
        // Caso de erro
         } else {
            printf("Não foi possível identificar quem chegou. (Seed problem)\n");
         }

    }

    pthread_join(threadBarco,NULL);

    printf("\n\nAcabram as viagens de hoje! Volte amanhã para mais!\n\n");
    return 0;
}