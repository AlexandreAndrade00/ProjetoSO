//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include "main.h"
#include "files.h"
#include "gestorCorrida.h"
#include "gestorAvarias.h"


//id memoria partilhada
int shmid, fd;

//variavel onde esta alocada a memoria
sharedMemory *sharedVar;

key_t key;

//SIMULADOR DE CORRIDA - PROCESSO PRINCIPAL
int main() {
    char buffer[255];

    initilization();

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        write(fd, buffer, strlen(buffer) + 1);
        if (strcmp(buffer, "END\n") == 0)
            break;
    }


    closeProgram();
    return 0;
}

//inicializacao da memoria partilhada e processos
void initilization() {
    int *configOptions;

    //ler ficheiro configuracoes
    if ((configOptions = readConfigFile()) == NULL)
        exit(-1);

    //calculo memoria a alocar
    unsigned long memorySize = sizeof(sharedMemory) + configOptions[3] * sizeof(nodeCarro) * configOptions[4] + sizeof(configOptions);

    //criacao da memoria partilhada
    if ((shmid = shmget(key, memorySize, IPC_CREAT | 0700))<0) {
        writeLogFile("erro a alocar memoria partilhada");
        exit(-1);
    }

    if((sharedVar = (sharedMemory *) shmat(shmid, NULL, 0))<0) {
        writeLogFile("erro a anexar memoria partilhada a memoria local");
        exit(-1);
    }

    //criacao de semaforos
    sem_unlink("MUTEXLOG");
    if ((sharedVar->mutexLog = sem_open("MUTEXLOG", O_CREAT | O_EXCL, 0700, 1))<0) {
        writeLogFile("erro a criar semaforo MUTEXLOG");
        exit(-1);
    }

    sem_unlink("RACE");
    if ((sem_open("RACE", O_CREAT | O_EXCL, 0700, 0))<0) {
        writeLogFile("erro a criar semaforo RACE");
        exit(-1);
    }

    //guardar array com configuracoes
    sharedVar->configOptions = configOptions;
    sharedVar->place = 1;
    sharedVar->avarias = 0;
    sharedVar->totalCarros = 0;

    //indicador de corrida
    sharedVar->race = false;
    sharedVar->stop = false;
    sharedVar->pause = false;

    //abrir ficheiro para escrever log's
    if ((sharedVar->logPtr = openLogFile()) == NULL) {
        writeLogFile("Sem permissoes para criar ficheiro de log!");
        exit(-1);
    }

    writeLogFile("SIMULATOR STARTING");
    fflush(sharedVar->logPtr);

    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);

    //criar processo gestor de corrida
    if (fork() == 0) {
        mainGestorCorrida();
        exit(0);
    }

    //criar processo gestor de avarias
    if (fork() == 0) {
        mainGestorAvarias();
        exit(0);
    }

    signal(SIGTSTP, sigtstp);
    signal(SIGINT, shutdown);

    unlink("fifo");
    mkfifo("fifo", 0600);
    if ((fd = open("fifo", O_WRONLY))<0) {
        writeLogFile("erro a abrir named pipe");
        exit(-1);
    }
}

//aguardar processos e desalocar memoria e semaforos
void closeProgram() {
    while (wait(NULL) > 0);

    close(fd);//desalocar memoria partilhada
    writeLogFile("SIMULATOR CLOSING");
    fclose(sharedVar->logPtr);                  //fechar ficheiro de log
    sem_close(sharedVar->mutexLog);             //fechar semaforo
    sem_unlink("MUTEXLOG");
    sem_unlink("MUTEXBOX");

    shmctl(shmid, IPC_RMID, NULL);
}
void sigtstp() {
    writeLogFile(" SIGNAL SIGTSTP RECEIVED");
    estat();
}

void estat() {
    int totalCarros = sharedVar->configOptions[3] * sharedVar->configOptions[4];
    nodeCarro listaCarros[totalCarros];
    memcpy(listaCarros, sharedVar->listaCarros, sizeof(listaCarros));
    char buffer[254];
    int i, j, totalParagens = sharedVar->listaCarros[0].paragensBox, carrosPista = 0;
    nodeCarro elem;

    if (sharedVar->listaCarros[0].estado != 1 && sharedVar->listaCarros[0].estado != 2)
        carrosPista++;

    //insertion sort
    for (i = 1; i < totalCarros; i++) {
        if (sharedVar->listaCarros[i].estado > 2)
            carrosPista++;
        totalParagens += sharedVar->listaCarros[i].paragensBox;
        elem = listaCarros[i];
        j = i - 1;
        while (j >= 0) {
            if (listaCarros[j].place > elem.place && elem.estado!=0) {
                listaCarros[j + 1] = listaCarros[j];
                j--;
            } else if (listaCarros[j].place == elem.place) {
                if (listaCarros[j].volta < elem.volta) {
                    listaCarros[j + 1] = listaCarros[j];
                    j--;
                } else if (listaCarros[j].volta == elem.volta) {
                    if (listaCarros[j].dist < elem.dist) {
                        listaCarros[j + 1] = listaCarros[j];
                        j--;
                    } else break;
                } else break;
            } else break;
        }
        listaCarros[j + 1] = elem;
    }

    writeLogFile("-------------------------------------ESTATISTICAS-----------------------------------------");
    for (i = 0; i < sharedVar->totalCarros && i < 5; i++) {
        snprintf(buffer, sizeof(buffer), "%dº LUGAR - Carro: %d, Equipa: %s, Voltas: %d, Paragens Box: %d", i + 1,
                 listaCarros[i].numCarro, listaCarros[i].team, listaCarros[i].volta, listaCarros[i].paragensBox);
        writeLogFile(buffer);
    }
    snprintf(buffer, sizeof(buffer), "ULTIMO LUGAR - Carro: %d, Equipa: %s, Voltas: %d, Paragens Box: %d",
             listaCarros[sharedVar->totalCarros - 1].numCarro, listaCarros[sharedVar->totalCarros - 1].team,
             listaCarros[sharedVar->totalCarros - 1].volta, listaCarros[sharedVar->totalCarros - 1].paragensBox);
    writeLogFile(buffer);

    snprintf(buffer, sizeof(buffer), "Total de avarias: %d", sharedVar->avarias);
    writeLogFile(buffer);
    snprintf(buffer, sizeof(buffer),"Total de abastecimentos: %d", totalParagens);
    writeLogFile(buffer);
    snprintf(buffer, sizeof(buffer),"Carros na pista: %d\n--------------------------------------------------------------------------------------------", carrosPista);
    writeLogFile(buffer);
}

void shutdown() {
    writeLogFile("SIGNAL SIGINT RECEIVED");
    char buffer[255];
    sharedVar->stop = true;
    strcpy(buffer, "END\n");
    write(fd, buffer, strlen(buffer) + 1);
    closeProgram();
    exit(0);
}