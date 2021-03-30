#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/types.h>
#include "files.h"
#include "gestorCorrida.h"
#include "gestorAvarias.h"
#include "main.h"
#include "files.h"

//id memoria partilahada
int shmid;

//variavel onde esta alocada a memoria
sharedMemory *sharedVar;

key_t key;

//SIMULADOR DE CORRIDA - PROCESSO PRINCIPAL
int main(int argc, char *argv[]) {
    initilization();
    closeProgram();
    return 0;
}

//inicializacao da memoria partilhada e processos
void initilization() {

    //criacao da memoria partilhada
    shmid = shmget(key, sizeof(sharedMemory), IPC_CREAT|0700);
    sharedVar = (sharedMemory*) shmat(shmid, NULL, 0);

    //criacao do semaforo
    sem_unlink("MUTEXLOG");
    sharedVar->mutexLog = sem_open("MUTEXLOG",O_CREAT|O_EXCL,0700,1);
    sem_unlink("MUTEXBOX");
    sharedVar->mutexBox = sem_open("MUTEXBOX",O_CREAT|O_EXCL,0700,1);

    //guardar array com configuracoes
    if ((sharedVar->configOptions = readConfigFile())==NULL)
        exit(-1);

    //abrir ficheiro para escrever log's
    if ((sharedVar->logPtr = openLogFile())==NULL) {
        printf("Sem permissoes para criar ficheiro de log!\n");
        exit(-1);
    }

    writeLogFile("SIMULATOR STARTING");
    fflush(sharedVar->logPtr);

    //criar processo gestor de corrida
    if (fork()==0) {
        mainGestorCorrida();
        exit(0);
    }

    //criar processo gestor de avarias
    if (fork()==0) {
        mainGestorAvarias();
        exit(0);
    }
}

//aguardar processos e desalocar memoria e semaforos
void closeProgram() {
    while (wait(NULL) > 0);

    shmctl(shmid, IPC_RMID, NULL);          //desalocar memoria partilhada
    writeLogFile("SIMULATOR CLOSING");
    fclose(sharedVar->logPtr);                  //fechar ficheiro de log
    sem_close(sharedVar->mutexLog);             //fechar semaforo
    sem_unlink("MUTEXLOG");
    sem_close(sharedVar->mutexBox);             //fechar semaforo
    sem_unlink("MUTEXBOX");
}