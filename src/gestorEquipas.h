//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#ifndef PROJETOSO_GESTOREQUIPAS_H
#define PROJETOSO_GESTOREQUIPAS_H

void mainGestorEquipas(int pos_equipa);

void *car(void *arguments);

void cleanup();

#endif //PROJETOSO_GESTOREQUIPAS_H
