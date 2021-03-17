//
// Created by alexandre on 17/03/21.
//

#include "SLinkedListManipulation.h"
#include <stdlib.h>
#include <string.h>

void newNode(nodeCarro *head, char team[], int num, int speed, float consum, int rel) {
    //alocar espaco para nodulo na memoria
    nodeCarro *temp = (nodeCarro *) malloc(sizeof(nodeCarro));

    //inserir informacoes do carro no nodulo
    strcpy(temp->team, team);
    temp->numCarro=num;
    temp->speed=speed;
    temp->consumption=consum;
    temp->reliability=rel;

    //caso a ainda nao exista nenhum elemento na lista
    if(head==NULL) {
        head=temp;
        head->next=NULL;
    } else { //colocar novo nodulo no inicio da lista
        temp->next=head;
        head=temp;
    }
}