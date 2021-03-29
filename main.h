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
    sem_t *mutexLog;
    int *configOptions;
    FILE *logPtr;
} sharedMemory;

extern sharedMemory *sharedVar;     //memoria partilhada

void initilization();

void closeProgram();

#endif //PROJETOSO_MAIN_H
