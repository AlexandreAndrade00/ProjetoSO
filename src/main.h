//FEITO POR ALEXANDRE ANDRADE - 2019220216

#ifndef PROJETOSO_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>


#define PROJETOSO_MAIN_H

typedef struct {
    //info dos carros
    char team[20];
    int numCarro;
    int speed;
    float consumption;
    int reliability;
    //atributos corrida
    int dist;
    float fuel;
    int volta;
    int place;
    bool broken;
    int estado;
    int paragensBox;
} nodeCarro;

typedef struct {
    int avarias;
    int terminou;
    bool race;
    bool stop;
    bool pause;
    int place;
    int totalCarros;
    sem_t *mutexLog;            //semaforo para escrever um de cada vez no ficheiro e stdout
    int *configOptions;         //array com info fo ficheiro de configuracoes
    FILE *logPtr;               //ficheiro log
    nodeCarro listaCarros[];     //array com carros
} sharedMemory;

extern sharedMemory *sharedVar; //memoria partilhada

void initilization();

void closeProgram();

void estat();

void shutdown();

void sigtstp();

#endif //PROJETOSO_MAIN_H
