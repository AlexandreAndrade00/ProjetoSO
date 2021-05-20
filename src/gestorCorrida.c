//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include "gestorCorrida.h"
#include "gestorEquipas.h"
#include "main.h"
#include "files.h"

int **pipeChannel;
char buffer[255];

//main do gestor de corridas
void mainGestorCorrida() {
    int numEquipas = sharedVar->configOptions[3];
    int numCarros = sharedVar->configOptions[4];
    char *string = NULL, *found = NULL, info[6][20];
    int fd, i, j, equipasExistentes = 0;
    sharedVar->terminou=0;
    fd_set read_set;
    sem_t *sem;

    if((sem = sem_open("RACE", 0))<0) {
        writeLogFile("erro a abrir semaforo RACE");
        exit(-1);
    }

    pipeChannel = (int **) malloc(sizeof(int *) * numEquipas);
    for (i = 0; i < numEquipas; i++) {
        pipeChannel[i] = (int *) malloc(sizeof(int) * 2);
        pipe(pipeChannel[i]);
    }

    signal(SIGUSR1, pauseRace);

    if((fd = open("fifo", O_RDONLY))<0) {
        writeLogFile("erro a abrir named pipe");
        exit(-1);
    }

    while (1) {
        i = 0;
        string = NULL;
        found = NULL;
        FD_ZERO(&read_set);
        FD_SET(fd, &read_set);
        for (j = 0; j < numEquipas; j++)
            FD_SET(*pipeChannel[j], &read_set);

        if (select(fd + 1, &read_set, NULL, NULL, NULL) > 0) {
            if (FD_ISSET(fd, &read_set)) {
                int wtv = read(fd, buffer, sizeof(buffer));
                buffer[wtv - 1] = '\0';

                string = strdup(buffer);

                //separar linha e ler valores
                while ((found = strsep(&string, " ")) != NULL) {
                    strcpy(info[i], found);
                    i++;
                }

                if (strcmp(info[0], "ADDCAR") == 0) {
                    if (sharedVar->race == false && sharedVar->pause == false) {
                        if (i == 6) {
                            for (j = 0; j < numEquipas * numCarros; j += numCarros) {
                                if (strcmp(sharedVar->listaCarros[j].team, "") == 0) {
                                    equipasExistentes++;
                                    if (fork() == 0) {
                                        mainGestorEquipas(j);
                                        exit(0);
                                    }
                                    guardarCarro(info[1], info[2], info[3], info[4], info[5], j);
                                    break;
                                } else if (strcmp(sharedVar->listaCarros[j].team, info[1]) == 0) {
                                    guardarCarro(info[1], info[2], info[3], info[4], info[5], j);
                                    break;
                                }
                            }
                            if (j == numEquipas * numCarros)
                                writeLogFile("Limite maximo de equipas atingido!");
                        } else {
                            char errorMsg[i * 20];
                            strcpy(errorMsg, buffer);
                            errorMsg[strcspn(errorMsg, "\n")] = 0;
                            snprintf(buffer, 255, "WRONG COMMAND => %s %d", errorMsg, i);
                            writeLogFile(buffer);
                        }
                    } else
                        writeLogFile("Rejected, race already started!");
                } else if (strcmp(buffer, "START RACE!\n") == 0) {
                    if (equipasExistentes==sharedVar->configOptions[3]) {
                        if (sharedVar->pause == false) {
                            for (i = 0; i < numCarros * numEquipas; i++) {
                                if (sharedVar->listaCarros[i].numCarro != 0) {
                                    sharedVar->listaCarros[i].estado = 5;
                                    sharedVar->listaCarros[i].volta = 0;
                                    sharedVar->listaCarros[i].dist = 0;
                                    sharedVar->listaCarros[i].fuel = (float) sharedVar->configOptions[8];
                                    sharedVar->listaCarros[i].broken = 0;
                                    sharedVar->listaCarros[i].paragensBox = 0;
                                    sharedVar->listaCarros[i].place = INT_MAX;
                                }
                            }
                            sharedVar->avarias=0;
                        } else
                            sharedVar->pause = false;
                        sharedVar->race = true;
                        for (j = 0; j < equipasExistentes; j++)
                            sem_post(sem);
                    } else
                        writeLogFile("CANNOT START, NOT ENOUGH TEAMS");
                } else if (strcmp(info[0], "END\n") == 0) {
                    if(sharedVar->race==false)
                        kill(0, SIGUSR2);
                }

                for (int m = 0; m < i; m++) {
                    info[m][0] = '\0';
                }
            }

            for (j = 0; j < numEquipas; j++) {
                if (FD_ISSET(*pipeChannel[j], &read_set)) {
                    read(pipeChannel[j][0], &i, sizeof(int));

                    if (i == -1) {
                        sharedVar->terminou++;
                    } else {
                        int estado = sharedVar->listaCarros[i].estado;
                        if (estado == 1) {
                            sharedVar->listaCarros[i].place = sharedVar->place;
                            snprintf(buffer, sizeof(buffer), "CARRO %d DA EQUIPA %s TERMINOU A CORRIDA EM %dº LUGAR!",
                                     sharedVar->listaCarros[i].numCarro, sharedVar->listaCarros[i].team,
                                     sharedVar->place++);
                            writeLogFile(buffer);
                            sharedVar->terminou++;
                        } else if (estado == 2) {
                            snprintf(buffer, sizeof(buffer), "CARRO %d DA EQUIPA %s FICOU SEM COMBUSTIVEL!",
                                     sharedVar->listaCarros[i].numCarro, sharedVar->listaCarros[i].team);
                            writeLogFile(buffer);
                            sharedVar->terminou++;
                        } else {
                            snprintf(buffer, sizeof(buffer), "CARRO %d DA EQUIPA %s MUDOU PARA O ESTADO %s",
                                     sharedVar->listaCarros[i].numCarro, sharedVar->listaCarros[i].team,
                                     getEstado(sharedVar->listaCarros[i].estado));
                            writeLogFile(buffer);
                        }
                    }
                    if (sharedVar->terminou == sharedVar->totalCarros) {
                        sharedVar->race=false;
                        for (j = 0; j < sharedVar->totalCarros; j++)
                            sem_post(sem);
                        if (sharedVar->pause==false)
                            sharedVar->place=1;
                        sharedVar->terminou=0;
                        if (sharedVar->stop==false)
                            estat();
                    }
                }
            }
            if (sharedVar->stop==true && sharedVar->race==false)
                break;
        }
    }

    while (wait(NULL) > 0);
    estat();
    for (i = 0; i < numEquipas; i++)
        free(pipeChannel[i]);
    sem_close(sem);
    sem_unlink("RACE");
    free(pipeChannel);
    free(string);
    close(fd);
    exit(0);
}

void guardarCarro(char team[], char num[], char speed[], char consum[], char rel[], int pos_equipa) {
    nodeCarro *car;
    for (int index = pos_equipa; index < pos_equipa + sharedVar->configOptions[4]; index++) {
        if (sharedVar->listaCarros[index].numCarro == 0) {
            car = &sharedVar->listaCarros[index];
            strcpy(sharedVar->listaCarros[index].team, team);
            car->numCarro = (int) strtol(num, (char **) NULL, 10);
            car->speed = (int) strtol(speed, (char **) NULL, 10);
            car->consumption = strtof(consum, (char **) NULL);
            car->reliability = (int) strtol(rel, (char **) NULL, 10);
            car->dist = 0;
            car->fuel = (float) sharedVar->configOptions[8];
            car->volta = 0;
            car->broken = false;
            car->estado = 5;
            car->paragensBox = 0;
            car->place=INT_MAX;
            sharedVar->totalCarros++;
            snprintf(buffer, 255, "NEW CAR LOADED => TEAM: %s, CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d",
                     car->team,
                     car->numCarro, car->speed, car->consumption, car->reliability);
            writeLogFile(buffer);
            return;
        }
    }
    snprintf(buffer, sizeof(buffer), "Equipa %s cheia!", sharedVar->listaCarros[pos_equipa].team);
    writeLogFile(buffer);
}

void pauseRace() {
    writeLogFile(" SIGNAL SIGUSR1 RECEIVED!");
    if (sharedVar->race==true)
        sharedVar->pause=true;
    else
        writeLogFile(" NENHUMA CORRIDA ATIVA!");
}

char* getEstado(int estado) {
    if (estado == 3)
        return "BOX";
    else if (estado==4)
        return "SEGURANCA";
    else if (estado==5)
        return "CORRIDA";
    else {
        printf("estado inexistente\n");
        exit(-1);
    }
}