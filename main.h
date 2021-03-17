//
// Created by alexandre on 17/03/21.
//

#include "SLinkedListManipulation.h"

#ifndef PROJETOSO_MAIN_H
#define PROJETOSO_MAIN_H

typedef struct {
    nodeCarro *listaCarros;
    int *configOptions;
} sharedMemory;

//variavel onde esta alocada a memoria
extern sharedMemory *sharedVar;

void initilization();

#endif //PROJETOSO_MAIN_H
