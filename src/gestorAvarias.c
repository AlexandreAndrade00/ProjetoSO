//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include "gestorAvarias.h"
#include "main.h"
#include "files.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/msg.h>

int mqid;

//main do gestor de avarias
void mainGestorAvarias() {
    key_t key;
    message msg;
    char buffer[254];
    srand(time(NULL));
    int random;
    writeLogFile("Gestor de avarias operacional!");
    key = ftok("config", 32);
    mqid = msgget(key, 0666 | IPC_CREAT);

    signal(SIGINT, shutdown_avarias);

    while(1) {
        while (sharedVar->race == true) {
            for (int i = 0; i < sharedVar->configOptions[3] * sharedVar->configOptions[4]; i++) {
                if (sharedVar->listaCarros[i].numCarro != 0) {
                    random = rand() % 100;
                    if (random >= sharedVar->listaCarros[i].reliability && sharedVar->listaCarros[i].broken == false && sharedVar->listaCarros[i].estado > 3) {
                        msg.car = i + 1;
                        msg.broken = true;
                        msgsnd(mqid, &msg, sizeof(message), 0);
                        snprintf(buffer, sizeof(buffer), "AVARIA NO CARRO %d DA EQUIPA %s!",
                                 sharedVar->listaCarros[i].numCarro, sharedVar->listaCarros[i].team);
                        writeLogFile(buffer);
                        sharedVar->avarias++;
                    }
                }
            }
            sleep(sharedVar->configOptions[5]);
        }
        sleep(1);
    }
}

void shutdown_avarias() {
    msgctl(mqid, IPC_RMID, NULL);
    exit(0);
}