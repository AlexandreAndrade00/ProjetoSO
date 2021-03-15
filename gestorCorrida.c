//
// Created by alexandre on 15/03/21.
//

#include "gestorCorrida.h"
#include "gestorEquipas.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//main do gestor de corridas
void mainGestorCorridas(const int *configOptions) {

    //inicializacao de um gestor de equipa para cada equipa
    for(int i=0; i<*(configOptions + 3); i++) {
        if (fork()==0) {
            mainGestorEquipas(configOptions);
            exit(0);
        }
    }

    printf("Estou sozinho");
    sleep(5);
}