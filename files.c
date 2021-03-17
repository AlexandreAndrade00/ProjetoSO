#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"

//ler ficheiro com configuracoes
int* readConfigFile() {
    FILE *fptr;
    char buffer[255];
    static int configOptions[8];
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
    return configOptions;
}