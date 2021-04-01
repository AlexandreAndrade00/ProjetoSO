//
// Created by alexandre on 17/03/21.
//

#include <stdio.h>
#include <semaphore.h>

#ifndef PROJETOSO_MAIN_H
#define PROJETOSO_MAIN_H

typedef struct {
    char team[20];
    int numCarro;
    int speed;                  //info dos carros
    float consumption;
    int reliability;
} nodeCarro;

typedef struct {
    int boxState;               //estado da box; 2-livre 1-reservada 0-ocupada
} nodeTeam;

typedef struct {
    nodeCarro *listaCarros;     //array com carros
    nodeTeam *boxList;          //array com boxes
    int freeIndexCar;           //index livre no array dos carros
    int freeIndexBox;           //index livre no array das boxes
    sem_t *mutexLog;            //semaforo para escrever um de cada vez no ficheiro e stdout
    sem_t *mutexBox;            //semaforo para um de cada vez entrar no array das boxes
    int *configOptions;         //array com info fo ficheiro de configuracoes
    FILE *logPtr;               //ficheiro log
} sharedMemory;

extern sharedMemory *sharedVar; //memoria partilhada

void initilization();

void closeProgram();

#endif //PROJETOSO_MAIN_H
