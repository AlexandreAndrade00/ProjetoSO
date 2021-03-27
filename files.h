//
// Created by alexandre on 15/03/21.
//
#include <stdio.h>

#ifndef PROJETOSO_FILES_H
#define PROJETOSO_FILES_H

int* readConfigFile();

FILE* openLogFile();

void writeLogFile(char string[]);

int accessConfigOptions(int index);

#endif //PROJETOSO_FILES_H
