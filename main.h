//
// Created by alexandre on 17/03/21.
//

#include <stdio.h>
#include <semaphore.h>

#ifndef PROJETOSO_MAIN_H
#define PROJETOSO_MAIN_H
#define MAX_CARS 1024
#define MAX_TEAMS 128

typedef struct nodeCarro {
    char team[20];
    int numCarro;
    int speed;
    float consumption;
    int reliability;
} nodeCarro;

typedef struct {
    int boxState;
} nodeTeam;

typedef struct {
    nodeCarro listaCarros[MAX_CARS];
    int freeIndexCar;
    nodeTeam boxList[MAX_TEAMS];
    int freeIndexBox;
    sem_t *mutexLog;
    sem_t *mutexBox;
    int *configOptions;
    FILE *logPtr;
} sharedMemory;

extern sharedMemory *sharedVar;     //memoria partilhada

void initilization();

void closeProgram();

#endif //PROJETOSO_MAIN_H
