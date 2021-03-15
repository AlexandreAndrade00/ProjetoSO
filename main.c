#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include "files.h"
#include "gestorCorrida.h"
#include "gestorAvarias.h"

void initilization();

typedef struct {
    //TODO
    int var;
} sharedMemory;

int shmid;
sharedMemory *sharedVar;
int *configOptions;

//SIMULADOR DE CORRIDA - PROCESSO PRINCIPAL
int main(int argc, char *argv[]) {

    initilization();

    printf("Simulador de corrida operacional!\n");
    sleep(10);

    //desalocar memoria partilhada
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

//inicializacao da memoria partilhada e processos
void initilization() {

    //criacao da memoria partilhada
    shmid = shmget("SHM", sizeof(sharedMemory), IPC_CREAT|0700);
    sharedVar = (sharedMemory*) shmat(shmid, NULL, 0);

    //array com configuracoes
    configOptions = readConfigFile();

    //criar processo gestor de corrida
    if (fork()==0) {
        mainGestorCorridas(configOptions);
        exit(0);
    }

    //criar processo gestor de avarias
    if (fork()==0) {
        mainGestorAvarias(configOptions);
        exit(0);
    }
}

