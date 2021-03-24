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
#include "SLinkedListManipulation.h"
#include "main.h"
#include "files.h"

//id memoria partilahada
int shmid;

//variavel onde esta alocada a memoria
sharedMemory *sharedVar;

sem_t *mutexLog;

FILE *logPtr;

int *configOptions;

//SIMULADOR DE CORRIDA - PROCESSO PRINCIPAL
int main(int argc, char *argv[]) {

    initilization();
    sleep(1);

    shmctl(shmid, IPC_RMID, NULL);          //desalocar memoria partilhada
    writeLogFile("SIMULATOR CLOSING");
    fclose(logPtr);                             //fechar ficheiro de log
    sem_close(mutexLog);                        //fechar semaforo
    sem_unlink("MUTEXLOG");

    return 0;
}

//inicializacao da memoria partilhada e processos
void initilization() {

    //criacao da memoria partilhada
    shmid = shmget("SHM", sizeof(sharedMemory), IPC_CREAT|0700);
    sharedVar = (sharedMemory*) shmat(shmid, NULL, 0);

    //criacao do semaforo
    sem_unlink("MUTEXLOG");
    mutexLog = sem_open("MUTEXLOG",O_CREAT|O_EXCL,0700,1);

    //guardar array com configuracoes
    configOptions = readConfigFile();

    //abrir ficheiro para escrever log's
    logPtr = openLogFile();

    writeLogFile("SIMULATOR STARTING");
    fflush(logPtr);
    sleep(1);

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