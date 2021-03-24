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
sem_t *mutexConfig;

FILE *logPtr;

int *configOptions;

pid_t corridaPID, avariasPID;

key_t key;

//SIMULADOR DE CORRIDA - PROCESSO PRINCIPAL
int main(int argc, char *argv[]) {

    initilization();
    waitpid(corridaPID, NULL, 0);
    waitpid(avariasPID, NULL, 0);

    shmctl(shmid, IPC_RMID, NULL);          //desalocar memoria partilhada
    writeLogFile("SIMULATOR CLOSING");
    fclose(logPtr);                             //fechar ficheiro de log
    sem_close(mutexLog);                        //fechar semaforo
    sem_close(mutexConfig);
    sem_unlink("MUTEXLOG");
    sem_unlink("MUTEXCONFIG");

    return 0;
}

//inicializacao da memoria partilhada e processos
void initilization() {

    //criacao da memoria partilhada
    shmid = shmget(key, sizeof(sharedMemory), IPC_CREAT|0700);
    sharedVar = (sharedMemory*) shmat(shmid, NULL, 0);

    //criacao do semaforo
    sem_unlink("MUTEXLOG");
    mutexLog = sem_open("MUTEXLOG",O_CREAT|O_EXCL,0700,1);

    sem_unlink("MUTEXCONFIG");
    mutexConfig = sem_open("MUTEXCONFIG",O_CREAT|O_EXCL,0700,1);

    //guardar array com configuracoes
    configOptions = readConfigFile();

    //abrir ficheiro para escrever log's
    logPtr = openLogFile();

    writeLogFile("SIMULATOR STARTING");
    fflush(logPtr);

    //criar processo gestor de corrida
    if ((corridaPID=fork())==0) {
        mainGestorCorrida();
        exit(0);
    }

    //criar processo gestor de avarias
    if ((avariasPID=fork())==0) {
        mainGestorAvarias();
        exit(0);
    }
}