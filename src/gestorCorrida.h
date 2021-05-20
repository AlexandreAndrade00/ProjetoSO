//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include <pthread.h>
#include <limits.h>

#ifndef PROJETOSO_GESTORCORRIDA_H
#define PROJETOSO_GESTORCORRIDA_H

extern int **pipeChannel;

void mainGestorCorrida();

void guardarCarro(char team[], char num[], char speed[], char consum[], char rel[], int pipe_index);

void pauseRace();

char* getEstado(int estado);

#endif //PROJETOSO_GESTORCORRIDA_H