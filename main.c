#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256
#define NUM_REGS 32
#define MAX_INSTRUCTIONS 100

// Estrutura para representar uma instrução
typedef struct {
    char nomeInstrucao[5]; // Nome da instrução
    int end1, end2, end3;   // Registradores e endereço
    int dado1, dado2, dado3; // Dados dos registradores
    bool Valido;            // Indica se a instrução é válida
} instrucaoFim;

// Variáveis globais
char txtInstrucoes[MAX_INSTRUCTIONS][MAX_LENGTH]; // Instruções lidas do arquivo
int R[NUM_REGS] = {0}; // Registradores
instrucaoFim instrucoes[MAX_INSTRUCTIONS]; // Array de instruções
int numInt = 0; // Número de instruções lidas
int pc = 0; // Contador de programa
int memory[256] = {0}; // Memória de dados

// Função para ler o arquivo com as instruções
void reader() {
    char *filename = "programa.txt";
    FILE *fp = fopen(filename, "r"); // Abre o arquivo para leitura

    if (fp == NULL) {
        printf("Error: could not open file %s\n", filename);
        return;
    }

    while (fgets(txtInstrucoes[numInt], MAX_LENGTH, fp) && numInt < MAX_INSTRUCTIONS) {
        if (strncmp(txtInstrucoes[numInt], "noop", 4) == 0 || strncmp(txtInstrucoes[numInt], "halt", 4) == 0) {
            sscanf(txtInstrucoes[numInt], "%s", instrucoes[numInt].nomeInstrucao);
            instrucoes[numInt].nomeInstrucao[0] = 0; // Marcar instrução como vazia
            instrucoes[numInt].end1 = 0; // Inicializa os registradores e endereços
            instrucoes[numInt].end2 = 0;
            instrucoes[numInt].end3 = 0;
            instrucoes[numInt].Valido = true; // Marca a instrução como válida
            numInt++;
        } else {
            sscanf(txtInstrucoes[numInt], "%s %d %d %d",
                   instrucoes[numInt].nomeInstrucao,
                   &instrucoes[numInt].end1,
                   &instrucoes[numInt].end2,
                   &instrucoes[numInt].end3);
            instrucoes[numInt].Valido = true; // Marca a instrução como válida
            numInt++;
        }
    }

    fclose(fp); // Fecha o arquivo após a leitura
}

// Estágio 1: Busca da instrução
instrucaoFim fetch(int pc) {
    if (pc < numInt) {
        return instrucoes[pc]; // Retorna a instrução do contador de programa
    }
    return (instrucaoFim){0}; // Retorna uma instrução vazia
}

// Estágio 2: Decodificação da instrução
void decode(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0 || !instr->Valido) return; // Verifica se a instrução é vazia ou inválida

    instr->dado1 = R[instr->end1]; // Carrega dados do primeiro registrador
    instr->dado2 = R[instr->end2]; // Carrega dados do segundo registrador
    instr->dado3 = instr->end3; // Carrega o endereço

    printf("Decodificando: %s, Dado1: %d, Dado2: %d, Dado3: %d\n",
           instr->nomeInstrucao, instr->dado1, instr->dado2, instr->dado3);
}

// Estágio 3: Execução da instrução
int execute(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0 || !instr->Valido) return 0; // Verifica se a instrução é vazia ou inválida

    if (strcmp(instr->nomeInstrucao, "add") == 0) {
        return instr->dado2 + instr->dado3; // Soma
    } else if (strcmp(instr->nomeInstrucao, "sub") == 0) {
        return instr->dado2 - instr->dado3; // Subtração
    } else if (strcmp(instr->nomeInstrucao, "beq") == 0) {
        return (instr->dado1 == instr->dado2) ? 1 : 0; // Retorna 1 se igual
    }
    
    return 0; // Caso padrão
}

// Estágio 4: Acesso à memória
int memory_access(instrucaoFim *instr) {
    if (!instr->Valido) return 0; // Verifica se a instrução é válida

    if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        return memory[instr->end3]; // Carrega valor da memória para o registrador
    } else if (strcmp(instr->nomeInstrucao, "sw") == 0) {
        memory[instr->end3] = instr->dado1; // Armazena valor do registrador na memória
        return -1; // Indica que foi uma operação de armazenamento
    }
    return 0; // Nenhuma operação válida
}

// Estágio 5: Escrita no banco de registradores
void write_back(instrucaoFim *instr, int result) {
    if (instr->nomeInstrucao[0] == 0 || !instr->Valido) return; // Verifica se a instrução é vazia ou inválida

    if (result != -1) {
        R[instr->end1] = result; // Escreve o resultado no registrador
        printf("Escrevendo R[%d] = %d\n", instr->end1, result);
    }
}

int main() {
    instrucaoFim pipes[4] = {0}; // Pipeline com 4 estágios
    memory[10] = 100; // Inicializa memória
    memory[11] = 254;

    reader(); // Lê as instruções do arquivo

    int result = 0;
    // Ciclo do pipeline, iterando enquanto houver instruções válidas ou ainda no pipeline
    while (pc < numInt || pipes[3].nomeInstrucao[0] != 0 || pipes[2].nomeInstrucao[0] != 0 ||
           pipes[1].nomeInstrucao[0] != 0 || pipes[0].nomeInstrucao[0] != 0) {
        
        // Estágio 5: Escrita no banco de registradores
        write_back(&pipes[3], result); 

        // Estágio 4: Acesso à memória
        result = memory_access(&pipes[2]); 
        
        // Estágio 3: Execução da instrução
        result = execute(&pipes[2]); 
        
        // Estágio 2: Decodificação da instrução
        decode(&pipes[1]); 
        
        // Estágio 1: Busca da próxima instrução
        pipes[0] = fetch(pc++); 

        // Avança o pipeline
        pipes[3] = pipes[2]; // Escreve
        pipes[2] = pipes[1]; // Acesso à memória
        pipes[1] = pipes[0]; // Decodifica
    }

    // Imprime o valor dos registradores
    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%d] = %d\n", i, R[i]);
    }

    return 0;
}
