//
// Created by alexandre on 15/03/21.
//

#include "gestorEquipas.h"
#include "files.h"
#include "main.h"
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

//main do gestor de equipas
void mainGestorEquipas() {

    int numCarros = sharedVar->configOptions[4];
    int myIndex;

    sem_wait(sharedVar->mutexBox);
    myIndex=sharedVar->freeIndexBox;
    sharedVar->freeIndexBox++;
    sem_post(sharedVar->mutexBox);
    sharedVar->boxList[myIndex].boxState=2;

    pthread_t my_thread[numCarros];

    writeLogFile("Gestor de Equipas operacional!");


    for(int i=0; i < numCarros; i++)
        pthread_create(&my_thread[i], NULL, car, NULL);

    for(int i=0; i < numCarros; i++)
        pthread_join(my_thread[i], NULL);
}

void *car(void *arguments) {
    writeLogFile("Carro inicializado");
}