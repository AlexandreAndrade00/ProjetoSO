//FEITO POR ALEXANDRE ANDRADE - 2019220216

#include "main.h"
#include <stdio.h>
#include <time.h>

#ifndef PROJETOSO_FILES_H
#define PROJETOSO_FILES_H

int* readConfigFile();

FILE* openLogFile();

void writeLogFile(char string[]);

#endif //PROJETOSO_FILES_H
