//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include "gestorEquipas.h"
#include "main.h"
#include "files.h"
#include "gestorCorrida.h"
#include "gestorAvarias.h"

int estadoBox = 2;
pthread_t *threads;
sem_t mutexBox;
sem_t pipeSem;
sem_t mudouEstadoCarro;
sem_t naBox;
sem_t *sem;
int index_equipa;
int numCarros;
int numThreads;

//main do gestor de equipas
void mainGestorEquipas(int pos_equipas) {
    index_equipa=pos_equipas;
    sem_init(&mutexBox, 1, 1);
    sem_init(&pipeSem, 1, 1);
    sem_init(&naBox, 1, 0);
    sem_init(&mudouEstadoCarro, 1, 0);

    if((sem = sem_open("RACE", 0))<0) {
        printf("erro a abrir semaforo RACE\n");
        exit(-1);
    }

    int i;
    numThreads = 0;
    numCarros = sharedVar->configOptions[4];
    threads = malloc(sizeof(pthread_t) * sharedVar->configOptions[4]);
    signal(SIGUSR2, cleanup);
    signal(SIGUSR1, SIG_IGN);

    writeLogFile("Gestor de Equipas operacional!");
    while (1) {
        sem_wait(sem);
        for (i = 0; i < numCarros; i++) {
            int *arg = malloc(sizeof(*arg));
            if (sharedVar->listaCarros[pos_equipas + i].numCarro != 0) {
                *arg=i+pos_equipas;
                pthread_create(&threads[i], NULL, car, arg);
                numThreads++;
            }
        }

        while(sharedVar->race==true) {
            sem_wait(&mudouEstadoCarro);
            for (i = 0; i < numCarros; i++) {
                if (sharedVar->listaCarros[pos_equipas + i].estado == 3) {
                    estadoBox = 0;
                    break;
                } else if ((sharedVar->listaCarros[pos_equipas + i].estado == 4) && (estadoBox==2)) {
                    estadoBox = 1;
                    break;
                }
            }
            sem_post(&mutexBox);

            if (sharedVar->listaCarros[pos_equipas + i].estado == 3) {
                sharedVar->listaCarros[pos_equipas + i].fuel = (float) sharedVar->configOptions[8];
                sharedVar->listaCarros[pos_equipas + i].paragensBox++;
                if (sharedVar->listaCarros[pos_equipas + i].broken == true) {
                    sharedVar->listaCarros[pos_equipas + i].broken = false;
                    usleep(((double) ((rand() % (sharedVar->configOptions[7] - sharedVar->configOptions[6] + 1)) +
                                      sharedVar->configOptions[6]) / (double) sharedVar->configOptions[0]) *
                           1000000);
                }
                usleep(((double) 2 / (double) sharedVar->configOptions[0]) * 1000000);
                sem_post(&naBox);
                estadoBox = 2;
            }
        }

        for (i = 0; i < numThreads; i++)
            pthread_join(threads[i], NULL);

        for (i = 0; i < numThreads-1; i++)
            sem_wait(&mudouEstadoCarro);
        sem_wait(&mutexBox);

        if (sharedVar->stop == true)
            break;

        numThreads = 0;
    }

    cleanup();
    exit(0);
}

void *car(void *arguments) {
    int myCar = *((int *) arguments);
    int lastState = 5, mqid;
    nodeCarro *car;
    message msg;
    msg.broken = false;
    srand(time(NULL));
    car = &sharedVar->listaCarros[myCar];
    key_t key;
    key = ftok("config", 32);
    mqid = msgget(key, 0666 | IPC_CREAT);

    while (sharedVar->race == true) {
        msgrcv(mqid, &msg, sizeof(msg), myCar + 1, IPC_NOWAIT);
        switch (car->estado) {
            case 5:
                car->dist += car->speed;
                car->fuel -= car->consumption;
                if (car->dist >= sharedVar->configOptions[1]) {
                    car->dist = car->dist - sharedVar->configOptions[1];
                    car->volta++;
                }
                sem_wait(&mutexBox);
                if (msg.broken == true) {
                    car->estado = 4;
                    sem_post(&mudouEstadoCarro);
                    car->broken = true;
                    msg.broken = false;
                } else if (((car->fuel * car->speed) / car->consumption) <= sharedVar->configOptions[1] * 2) {
                    car->estado = 4;
                    sem_post(&mudouEstadoCarro);
                } else if (((car->fuel * car->speed) / car->consumption) <= sharedVar->configOptions[1] * 4) {
                    if (estadoBox == 2 && car->dist < car->speed) {
                        car->estado = 3;
                        sem_post(&mudouEstadoCarro);
                    } else
                        sem_post(&mutexBox);
                } else
                    sem_post(&mutexBox);
                break;
            case 4:
                car->dist += (car->speed) * 0.3;
                car->fuel -= (car->consumption) * 0.4;
                if (car->dist >= sharedVar->configOptions[1]) {
                    car->dist = car->dist - sharedVar->configOptions[1];
                    car->volta++;
                }
                if (msg.broken == true) {
                    car->broken = true;
                    msg.broken = false;
                }

                sem_wait(&mutexBox);
                if ((car->dist < (car->speed * 0.3)) && (estadoBox != 0)) {
                    car->estado = 3;
                    sem_post(&mudouEstadoCarro);
                } else if (car->fuel <= 0) {
                    car->estado = 2;
                    sem_post(&mudouEstadoCarro);
                } else
                    sem_post(&mutexBox);

                break;
            case 3:
                sem_wait(&naBox);
                car->estado = 5;
                break;
        }

        if (car->volta == sharedVar->configOptions[2]) {
            car->estado = 1;
        }
        printf("Carro %d na pos %d, na volta %d, no estado %d, combustivel: %f\n", car->numCarro, car->dist, car->volta,
               car->estado, car->fuel);

        if ((sharedVar->stop == true || sharedVar->pause == true) && (car->dist < car->speed)) {
            if (sharedVar->pause == false) {
                car->estado = 1;
                sem_wait(&pipeSem);
                write(pipeChannel[index_equipa/numCarros][1], &myCar, sizeof(int));
                sem_post(&pipeSem);
            } else {
                int temp = -1;
                sem_wait(&pipeSem);
                write(pipeChannel[index_equipa/numCarros][1], &temp, sizeof(int));
                sem_post(&pipeSem);
            }
            sem_wait(sem);
            sem_post(&mudouEstadoCarro);
            pthread_exit(0);
        }

        if (car->estado != lastState) {
            sem_wait(&pipeSem);
            write(pipeChannel[index_equipa/numCarros][1], &myCar, sizeof(int));
            sem_post(&pipeSem);
        }

        if (car->estado == 1 || car->estado ==2) {
            sem_wait(sem);
            sem_post(&mudouEstadoCarro);
            break;
        }

        lastState = car->estado;
        usleep(((double) 1 / (double) sharedVar->configOptions[0]) * 1000000);
    }

    pthread_exit(0);
}

void cleanup() {
    sem_destroy(&mutexBox);
    sem_destroy(&pipeSem);
    sem_destroy(&mudouEstadoCarro);
    sem_destroy(&naBox);
    free(threads);
    exit(0);
}