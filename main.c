#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256
#define NUM_REGS 32
#define MAX_INSTRUCTIONS 100

typedef struct {
    char nomeInstrucao[5];
    int end1, end2, end3;
    int dado1, dado2, dado3;
    bool Valido;
} instrucaoFim;

char txtInstrucoes[MAX_INSTRUCTIONS][255];

int R[NUM_REGS] = {0};
instrucaoFim instrucoes[MAX_INSTRUCTIONS];
int numInt = 0;
int pc = 0;
int memory[256] = {0};

void reader() {
    char *filename = "programa.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s\n", filename);
        return;
    }

    while (fgets(txtInstrucoes[numInt], MAX_LENGTH, fp) && numInt < MAX_INSTRUCTIONS) {
        if (strncmp(txtInstrucoes[numInt], "noop", 4) == 0 || strncmp(txtInstrucoes[numInt], "halt", 4) == 0) {
            sscanf(txtInstrucoes[numInt], "%s", instrucoes[numInt].nomeInstrucao);
            instrucoes[numInt].nomeInstrucao[0] = 0;
            instrucoes[numInt].end1 = 0;
            instrucoes[numInt].end2 = 0;
            instrucoes[numInt].end3 = 0;
            instrucoes[numInt].Valido = true;
            numInt++;
        } else {
            sscanf(txtInstrucoes[numInt], "%s %d %d %d",
                   instrucoes[numInt].nomeInstrucao,
                   &instrucoes[numInt].end1,
                   &instrucoes[numInt].end2,
                   &instrucoes[numInt].end3);
            instrucoes[numInt].Valido = true;
            numInt++;
        }
    }

    fclose(fp);
}

instrucaoFim fetch(int pc) {
    if (pc < numInt) {
        return instrucoes[pc];
    }
    return (instrucaoFim){0};
}

void decode(instrucaoFim *instr) {
    if(instr->nomeInstrucao[0] == 0) return;
    if(instr->Valido == false) return;

    instr->dado1 = R[instr->end1];
    instr->dado2 = R[instr->end2];
    instr->dado3 = R[instr->end3];
}

int execute(instrucaoFim *instr) {
    if(instr->nomeInstrucao[0] == 0) return 0;
    if(instr->Valido == false) return 0;

    if (strcmp(instr->nomeInstrucao, "add") == 0) {
        return instr->dado2 + instr->dado3;
    } else if (strcmp(instr->nomeInstrucao, "sub") == 0) {
        return instr->dado2 - instr->dado3;
    } else if (strcmp(instr->nomeInstrucao, "beq") == 0) {
        return (instr->dado1 == instr->dado2) ? 1 : 0;
    } else if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        return memory[instr->dado2 + instr->end3];
    } else if (strcmp(instr->nomeInstrucao, "sw") == 0) {
        memory[instr->end2 + instr->end3] = instr->dado1;
        return -1;
    }
    return 0;
}

void write_back(instrucaoFim *instr, int result) {
    if(instr->nomeInstrucao[0] == 0) return;
    if(instr->Valido == false) return;

    if (result != -1) {
        R[instr->end1] = result;
    }
}

int main() {
    instrucaoFim pipes[4] = {0};
    memory[10] = 100;
    memory[11] = 254;

    //Le arquivo do disco e salva em um array de instrucoes
    reader();

    int result = 0;
    //Entra a cada ciclo de clock
    //Fica iterando enquanto huver alguma instrucao valida, e pc for menor do que a quantidade de instrucoes
    while(pc < numInt || pipes[3].nomeInstrucao[0] != 0 || pipes[2].nomeInstrucao[0] != 0 || 
      pipes[1].nomeInstrucao[0] != 0 || pipes[0].nomeInstrucao[0] != 0) {
        write_back(&pipes[3], result);
        result = execute(&pipes[2]);
        decode(&pipes[1]);
        pipes[0] = fetch(pc);

        pipes[3] = pipes[2];
        pipes[2] = pipes[1];
        pipes[1] = pipes[0];
        pc++;
    }


    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%d] = %d\n", i, R[i]);
    }

    return 0;
}
