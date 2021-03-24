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
#include <sys/wait.h>

//main do gestor de corridas
void mainGestorCorrida() {

    //inicializacao de um gestor de equipa para cada equipa
    for(int i=0; i<*(configOptions + 3); i++) {
        if (fork()==0) {
            mainGestorEquipas();
            exit(0);
        }
    }
}

void guardarCarro(char team[], int num, int speed, float consum, int rel) {
    newNode(sharedVar->listaCarros, team, num, speed, consum, rel);
}