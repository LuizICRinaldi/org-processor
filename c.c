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
    char *filename = "outras_instrucoes.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s\n", filename);
        return;
    }

    while (fgets(txtInstrucoes[numInt], MAX_LENGTH, fp) && numInt < MAX_INSTRUCTIONS) {
        if (strncmp(txtInstrucoes[numInt], "noop", 4) == 0 || strncmp(txtInstrucoes[numInt], "halt", 4) == 0) {
            sscanf(txtInstrucoes[numInt], "%s", instrucoes[numInt].nomeInstrucao);
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

    instr->dado1 = R[instr->end1];
    instr->dado2 = R[instr->end2];
    instr->dado3 = R[instr->end3];
}

int execute(instrucaoFim instr) {

    if (strcmp(instr.nomeInstrucao, "add") == 0) {
        return instr.dado2 + instr.dado3;
    } else if (strcmp(instr.nomeInstrucao, "sub") == 0) {
        return instr.dado2 - instr.dado3;
    } else if (strcmp(instr.nomeInstrucao, "beq") == 0) {
        return (instr.dado2 == instr.dado3) ? 1 : 0;
    } else if (strcmp(instr.nomeInstrucao, "lw") == 0) {
        return memory[instr.end2 + instr.end3];
    } else if (strcmp(instr.nomeInstrucao, "sw") == 0) {
        memory[instr.end2 + instr.end3] = instr.dado1;
        return -1;
    }
    return 0;
}

void write_back(instrucaoFim instr, int result) {
    if (result != -1) {
        R[instr.end1] = result;
    }
}

int main() {
    memory[10] = 100;
    memory[11] = 254;

    reader();

    for (pc = 0; pc < numInt; pc++) {
        instrucaoFim instr = fetch(pc);
        decode(&instr);
        int result = execute(instr);

        write_back(instr, result);
    }

    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%d] = %d\n", i, R[i]);
    }

    return 0;
}
