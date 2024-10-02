#include "reader.h"

void reader() {
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
