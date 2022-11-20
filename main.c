#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_BOAT_VISITORS 20

#define ARRIVED_HACKER 0
#define ARRIVED_SERF 1

time_t t;

void *serf(void *args) {
    printf("Vai o serf\n");
    pthread_exit(NULL);
}

void *hacker(void *args) {
    printf("Vai o hacker\n");
    pthread_exit(NULL);
}

void *barco(void *args) {
    printf("\n\nIniciando as viagens do dia!\n\n");
    pthread_exit(NULL);
}

int main(void) {
    // Thread para o grupo de Hackers e Serfs que vão cruzar o Rio
    pthread_t threadHackers[MAX_BOAT_VISITORS];
    pthread_t threadSerfs[MAX_BOAT_VISITORS];

    // Thread do barco
    pthread_t threadBarco;

    int passengerArrival;

    srand((unsigned) time(&t));

    pthread_create(&threadBarco, NULL, barco, NULL);

    for(int i =0; i< MAX_BOAT_VISITORS; i++){
         passengerArrival = rand()%2;
         if (passengerArrival == ARRIVED_SERF){
            printf("Chegou um serf\n");
            pthread_create(&threadSerfs[i], NULL, serf, NULL);
         } else if (passengerArrival == ARRIVED_HACKER) {
            printf("Chegou um hacker\n");
            pthread_create(&threadHackers[i], NULL, hacker, NULL);
         } else {
            printf("Não foi possível identificar quem chegou. (Seed problem)\n");
         }

    }

    pthread_join(threadBarco,NULL);

    printf("\n\nAcabram as viagens de hoje! Volte amanhã para mais!\n\n");
    return 0;
}