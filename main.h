//
// Created by alexandre on 17/03/21.
//

#include "SLinkedListManipulation.h"
#include <stdio.h>
#include <semaphore.h>

#ifndef PROJETOSO_MAIN_H
#define PROJETOSO_MAIN_H

typedef struct {
    nodeCarro *listaCarros;
} sharedMemory;

extern sharedMemory *sharedVar;     //memoria partilhada
extern sem_t *mutexLog;             //semaforo
extern sem_t *mutexConfig;
extern FILE *logPtr;                //ficheiro log.txt
extern int *configOptions;          //array com configuracoes

void initilization();

void closeProgram();

#endif //PROJETOSO_MAIN_H
