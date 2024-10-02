#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 256

char instrucao[MAX_LENGTH];
int numInt = 0;
char memInstrucoes[MAX_LENGTH][MAX_LENGTH];

void reader();

#endif