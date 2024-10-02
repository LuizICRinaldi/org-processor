/*#include "main.h"

int main(int argc, char **argv) {

	reader();	
	
    return 0;
}*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256

typedef struct{
	char nomeInstrucao[5];
	int end1, end2, end3, dado1, dado2, dado3;
	bool Valido;	
} instrucaoFim;

char instrucao[MAX_LENGTH];
int numInt = 0;
char memInstrucoes[MAX_LENGTH][MAX_LENGTH];

void reader(){
	char *filename = "programa.txt";
    FILE *fp = fopen(filename, "r");
	int i = 0;

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
		return;
     
    }

    // reading line by line, max 256 bytes
	i = 0;
	while(fgets(memInstrucoes[numInt], MAX_LENGTH, fp)) {
		numInt++;
	} 	
	
	i = 0;
	while(strlen(&memInstrucoes[i][0])) {
		printf("%s", memInstrucoes[i]);
		i++;
		if(i >= MAX_LENGTH) break;
	}
    
    fclose(fp);
}

int main()
{
	reader();	

	return 0;
}
