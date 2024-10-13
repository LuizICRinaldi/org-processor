#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256
#define NUM_REGS 32
#define MAX_INSTRUCTIONS 100

// Estrutura que representa uma instrução no pipeline
typedef struct {
    char nomeInstrucao[5]; // Nome da instrução (ex: "add", "sub", "beq", etc.)
    int end1, end2, end3; // Endereços dos registradores ou offset
    int dado1, dado2, dado3; // Valores dos registradores
    bool Valido; // Indica se a instrução é válida
} instrucaoFim;

// Armazenamento das instruções lidas do arquivo
char txtInstrucoes[MAX_INSTRUCTIONS][255];

// Banco de registradores (R0 a R31)
int R[NUM_REGS] = {0};
instrucaoFim instrucoes[MAX_INSTRUCTIONS]; // Vetor para armazenar instruções
int numInt = 0; // Contador de instruções
int pc = 0; // Contador de programa
int memory[256] = {0}; // Memória de dados

// Função para ler o arquivo com as instruções
void reader() {
    char *filename = "programa.txt"; // Nome do arquivo contendo as instruções
    FILE *fp = fopen(filename, "r"); // Abre o arquivo

    if (fp == NULL) {
        printf("Error: could not open file %s\n", filename); // Mensagem de erro ao abrir o arquivo
        return;
    }

    // Leitura das instruções do arquivo
    while (fgets(txtInstrucoes[numInt], MAX_LENGTH, fp) && numInt < MAX_INSTRUCTIONS) {
        // Tratamento de instruções "noop" e "halt"
        if (strncmp(txtInstrucoes[numInt], "noop", 4) == 0 || strncmp(txtInstrucoes[numInt], "halt", 4) == 0) {
            sscanf(txtInstrucoes[numInt], "%s", instrucoes[numInt].nomeInstrucao);
            instrucoes[numInt].nomeInstrucao[0] = 0;
            instrucoes[numInt].end1 = 0;
            instrucoes[numInt].end2 = 0;
            instrucoes[numInt].end3 = 0;
            instrucoes[numInt].Valido = true; // Marcar como válida
            numInt++;
        } else {
            sscanf(txtInstrucoes[numInt], "%s %d %d %d",
                   instrucoes[numInt].nomeInstrucao,
                   &instrucoes[numInt].end1,
                   &instrucoes[numInt].end2,
                   &instrucoes[numInt].end3);
            instrucoes[numInt].Valido = true; // Marcar como válida
            numInt++;
        }
    }

    fclose(fp); // Fecha o arquivo
}

// Estágio 1: Busca da instrução
instrucaoFim fetch(int pc) {
    if (pc < numInt) {
        return instrucoes[pc]; // Retorna a instrução no endereço atual do PC
    }
    return (instrucaoFim){0}; // Retorna instrução vazia se o PC estiver fora do alcance
}

// Estágio 2: Decodificação da instrução
void decode(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0 || !instr->Valido) return; // Verifica se a instrução é válida

    // Lê os dados dos registradores
    instr->dado1 = R[instr->end1];
    instr->dado2 = R[instr->end2];
    instr->dado3 = R[instr->end3];

    printf("Decodificando: %s, Dado1: %d, Dado2: %d, Dado3: %d\n",
           instr->nomeInstrucao, instr->dado1, instr->dado2, instr->dado3);
}

// Estágio 3: Execução da instrução
int execute(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0 || !instr->Valido) return 0; // Verifica se a instrução é válida

    int result = 0; // Inicializa o resultado

    if (strcmp(instr->nomeInstrucao, "add") == 0) {
        result = instr->dado2 + instr->dado3; // Soma
    } else if (strcmp(instr->nomeInstrucao, "sub") == 0) {
        result = instr->dado2 - instr->dado3; // Subtração
    } else if (strcmp(instr->nomeInstrucao, "beq") == 0) {
        // Política "Não Tomada": apenas avança o PC se for falso
        if (instr->dado1 == instr->dado2) {
            // Se for verdadeira, retorna o novo PC (não implementado aqui)
            printf("BEQ: condição verdadeira, novo PC não implementado.\n");
            return 1; // Retorna um valor sinalizando que houve um desvio
        }
    }
    
    printf("Executando: %s, Resultado: %d\n", instr->nomeInstrucao, result);
    return result; // Retorna o resultado da execução
}

// Estágio 4: Acesso à memória
int memory_access(instrucaoFim *instr) {
    if (!instr->Valido) return 0; // Verifica se a instrução é válida

    if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        return memory[instr->end2 + instr->end3]; // Carrega da memória
    } else if (strcmp(instr->nomeInstrucao, "sw") == 0) {
        memory[instr->end2 + instr->end3] = instr->dado1; // Armazena na memória
        printf("SW: Armazenando R[%d] = %d no endereço %d\n", instr->end1, instr->dado1, instr->end2 + instr->end3);
        return -1; // Indica que foi uma operação de armazenamento
    }
    return 0; // Retorna 0 se não for uma instrução válida
}

// Estágio 5: Escrita no banco de registradores
void write_back(instrucaoFim *instr, int result) {
    if (instr->nomeInstrucao[0] == 0 || !instr->Valido) return; // Verifica se a instrução é válida

    if (result != -1) {
        R[instr->end1] = result; // Armazena o resultado no registrador de destino
        printf("Escrevendo R[%d] = %d\n", instr->end1, result);
    }
}

int main() {
    instrucaoFim pipes[4] = {0}; // Estruturas para armazenar instruções em cada estágio do pipeline
    memory[10] = 100; // Inicializa a memória com alguns valores
    memory[11] = 254;

    reader(); // Lê as instruções do arquivo

    int result = 0; // Inicializa o resultado
    // Ciclo do pipeline
    while (pc < numInt || pipes[3].nomeInstrucao[0] != 0 || pipes[2].nomeInstrucao[0] != 0 ||
           pipes[1].nomeInstrucao[0] != 0 || pipes[0].nomeInstrucao[0] != 0) {

        write_back(&pipes[3], result); // Estágio 5: WB
        result = memory_access(&pipes[2]); // Estágio 4: MEM
        result = execute(&pipes[2]); // Estágio 3: EX
        decode(&pipes[1]); // Estágio 2: ID
        pipes[0] = fetch(pc); // Estágio 1: IF

        // Avança o pipeline
        pipes[3] = pipes[2];
        pipes[2] = pipes[1];
        pipes[1] = pipes[0];
        pc++; // Avança o contador de programa
    }

    // Imprime o valor dos registradores
    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%d] = %d\n", i, R[i]);
    }

    return 0; // Finaliza o programa
}
