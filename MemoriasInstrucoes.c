#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256


typedef struct{
	char nomeInstrucao[5];
	int end1, end2, end3, dado1, dado2, dado3;
	bool Valido;	
}instrucaoFim;

char instrucao[MAX_LENGTH];
int numInt = 0;
char memInstrucoes[MAX_LENGTH][MAX_LENGTH];

void reader(){
	char *filename = "readme.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
     
    }

    // reading line by line, max 256 bytes

	while(	fgets(memInstrucoes[numInt], MAX_LENGTH, fp)){
		numInt++;
} 	
	
	int i;
	for(i = 0;i<4;i++){
		printf("%s", memInstrucoes[i]);
	}
  
    

    
    fclose(fp);
	
}

int main()
{
	char buffer[MAX_LENGTH];
	reader();	
	numInt=0;
	while(fgets(buffer, MAX_LENGTH, memInstrucoes[numInt])){
	printf("%s", buffer);
	} 
	
	
	printf("%s", buffer);
}
