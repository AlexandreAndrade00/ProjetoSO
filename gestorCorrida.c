//
// Created by alexandre on 15/03/21.
//

#include "gestorCorrida.h"
#include "gestorEquipas.h"
#include "main.h"
#include "SLinkedListManipulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//main do gestor de corridas
void mainGestorCorrida(const int *configOptions) {

    //inicializacao de um gestor de equipa para cada equipa
    for(int i=0; i<*(configOptions + 3); i++) {
        if (fork()==0) {
            mainGestorEquipas(configOptions);
            exit(0);
        }
    }

    sleep(5);
}

void guardarCarro(char team[], int num, int speed, float consum, int rel) {
    newNode(sharedVar->listaCarros, team, num, speed, consum, rel);
}