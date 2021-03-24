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

    sem_wait(mutexConfig);
    int numCarros = *(configOptions + 4);
    sem_post(mutexConfig);

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