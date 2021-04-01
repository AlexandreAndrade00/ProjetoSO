#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include "files.h"
#include "main.h"

//ler ficheiro com configuracoes
int* readConfigFile() {
    FILE *fptr;
    char buffer[255];
    static int configOptions[9];
    char *string, *found;
    int count=0, aux;

    //abrir ficheiro
    if ((fptr = fopen("config", "r")) == NULL) {
        printf("File doesn't exist!\n");
        return NULL;
    }

    //ler ficheiro linha a linha
    while (fgets(buffer, 255, fptr) != NULL) {
        string = strdup(buffer);

        //separar linha e ler valores
        while((found = strsep(&string, ", ")) != NULL) {
            aux = (int) strtol(found, (char **) NULL, 10);
            if (aux != 0) {
                configOptions[count] = aux;
                count++;
            }
        }
    }
    fclose(fptr);

    //verificar que foram lidos 9 int's
    if (count != 9) {
        printf("Ficheiro de configurações inválido!\n");
        exit(-1);

    //verificar se existem pelo menos 3 equipas
    } else if (configOptions[3] < 3) {
        printf("Equipas insuficientes!\n");
        exit(-1);
    }

    return configOptions;
}

//abrir ficheiro log.txt
FILE* openLogFile() {
    FILE *fptr;

    if ((fptr = fopen("log.txt", "w")) == NULL) {
        printf("Something wrong happen!\n");
        return NULL;
    }

    return fptr;
}

//escrever no ficheiro log.txt e stout
void writeLogFile(char string[]) {
    char write[512];
    time_t rawtime;
    struct tm * timeinfo;

    //atualizar tempo
    time(&rawtime);
    //passar tempo para estrutura
    timeinfo = localtime(&rawtime);

    //concatenar tempo com string de output
    sprintf(write, "[%d:%d:%d] %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, string);

    //escrever no stdout e log.txt
    sem_wait(sharedVar->mutexLog);
    printf("%s", write);
    fprintf(sharedVar->logPtr, "%s", write);
    sem_post(sharedVar->mutexLog);
}