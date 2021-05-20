//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include "files.h"

//ler ficheiro com configuracoes
int* readConfigFile() {
    FILE *fptr;
    char buffer[255];
    static int configOptions[9];
    char *string, *found;
    int count=0, aux;

    //abrir ficheiro
    if ((fptr = fopen("config", "r")) == NULL) {
        writeLogFile("File doesn't exist!");
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
        writeLogFile("Ficheiro de configurações inválido!");
        exit(-1);

    //verificar se existem pelo menos 3 equipas
    } else if (configOptions[3] < 3) {
        writeLogFile("Equipas insuficientes!");
        exit(-1);
    }

    return configOptions;
}

//abrir ficheiro log.txt
FILE* openLogFile() {
    FILE *fptr;

    if ((fptr = fopen("log.txt", "w")) == NULL) {
        writeLogFile("Something wrong happen!");
        return NULL;
    }

    return fptr;
}

//escrever no ficheiro log.txt e stout
void writeLogFile(char string[]) {
    char write[1024];
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