//
// Created by alexandre on 17/03/21.
//

#ifndef PROJETOSO_SLINKEDLISTMANIPULATION_H
#define PROJETOSO_SLINKEDLISTMANIPULATION_H

typedef struct nodeCarro {
    char team[20];
    int numCarro;
    int speed;
    float consumption;
    int reliability;
    struct nodeCarro *next;
} nodeCarro;

void newNode(nodeCarro *head, char team[], int num, int speed, float consum, int rel);

#endif //PROJETOSO_SLINKEDLISTMANIPULATION_H
