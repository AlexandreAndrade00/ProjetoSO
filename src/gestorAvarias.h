//FEITO POR ALEXANDRE ANDRADE - 2019220216

#ifndef PROJETOSO_GESTORAVARIAS_H
#define PROJETOSO_GESTORAVARIAS_H

#include <stdbool.h>


typedef struct {
    long car;
    bool broken;
} message;

void mainGestorAvarias();

void shutdown_avarias();

#endif //PROJETOSO_GESTORAVARIAS_H
