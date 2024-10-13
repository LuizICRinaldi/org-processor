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
int memory[256] = {0}; // Memória de dados

// Função para ler o arquivo com as instruções
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

// Estágio 1: Busca da instrução
instrucaoFim fetch(int pc) {
    if (pc < numInt) {
        return instrucoes[pc];
    }
    return (instrucaoFim){0};
}

// Estágio 2: Decodificação da instrução
void decode(instrucaoFim *instr) {
    if(instr->nomeInstrucao[0] == 0) return;
    if(instr->Valido == false) return;

    // Se for 'lw', carrega apenas o registrador de destino (end1) e o endereço (end3)
    if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        instr->dado1 = instr->end1;
        instr->dado2 = 0; // Não tem dado2 e dado3 em 'lw'
        instr->dado3 = instr->end3;
    } else {
        instr->dado1 = R[instr->end1];
        instr->dado2 = R[instr->end2];
        instr->dado3 = R[instr->end3];
    }

    printf("--------------------------------------------------\n");
    printf("Decodificando: %-4s | Dado1: %-3d | Dado2: %-3d | Dado3: %-3d\n",
           instr->nomeInstrucao, instr->dado1, instr->dado2, instr->dado3);
}

// Estágio 3: Execução da instrução
int execute(instrucaoFim *instr) {
    if(instr->nomeInstrucao[0] == 0) return 0;
    if(instr->Valido == false) return 0;

    int result = 0; // Inicializa a variável result
    if (strcmp(instr->nomeInstrucao, "add") == 0) {
        result = instr->dado2 + instr->dado3;
    } else if (strcmp(instr->nomeInstrucao, "sub") == 0) {
        result = instr->dado2 - instr->dado3;
    } else if (strcmp(instr->nomeInstrucao, "beq") == 0) {
        if (instr->dado1 == instr->dado2) {
            return instr->end3; // Retorna o novo endereço para o pc se a condição for verdadeira
        }
        return -1; // Retorna -1 se não for tomada
    }

    printf("Executando: %-4s | Resultado: %-4d\n", instr->nomeInstrucao, result);
    return result;
}

void write_back(instrucaoFim *instr, int result) {
    if (instr->nomeInstrucao[0] == 0) return;

    if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        R[instr->end1] = result; // Carrega o valor no registrador de destino
    } else if (strcmp(instr->nomeInstrucao, "add") == 0 || strcmp(instr->nomeInstrucao, "sub") == 0) {
        R[instr->end1] = result; // Escreve o resultado da operação
        printf("Escrevendo R[%d] = %d\n", instr->end1, result);
    }
}

int memory_access(instrucaoFim *instr) {
    if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        int endereco = instr->dado3; // Pega o endereço da memória
        instr->dado1 = memory[endereco]; // Carrega o valor da memória no registrador de destino
        printf("Escrevendo R[%d] = %d\n", instr->end1, instr->dado1);
        return instr->dado1; // Retorna o valor lido da memória
    }
    return 0;
}

int main() {
    instrucaoFim pipes[4] = {0};
    memory[10] = 100; // Preencher a memória
    memory[11] = 254;

    // Ler o arquivo e salvar em um array de instruções
    reader();

    int result = 0; // Resultado da execução

    // Loop do pipeline, itera enquanto houver instruções válidas ou ainda no pipeline
    while (pc < numInt || pipes[3].nomeInstrucao[0] != 0 || pipes[2].nomeInstrucao[0] != 0 ||
           pipes[1].nomeInstrucao[0] != 0 || pipes[0].nomeInstrucao[0] != 0) {

        // Estágio 5: Escrever de volta
        write_back(&pipes[3], result);

        // Estágio 4: Acesso à memória
        if (pipes[2].nomeInstrucao[0] != 0 && strcmp(pipes[2].nomeInstrucao, "lw") == 0) {
            result = memory_access(&pipes[2]); // Se for lw, pega o valor da memória
        } else {
            result = execute(&pipes[2]); // Estágio 3: Execução

            // Se a instrução foi 'beq' e a branch foi tomada, atualize o pc
            if (strcmp(pipes[2].nomeInstrucao, "beq") == 0 && result != -1) {
                pc = result; // Atualiza o pc para o novo valor
            }
        }

        // Estágio 2: Decodificação
        decode(&pipes[1]);

        // Estágio 1: Busca da instrução
        pipes[0] = fetch(pc); // Buscar a próxima instrução

        // Avançar o pipeline
        pipes[3] = pipes[2];
        pipes[2] = pipes[1];
        pipes[1] = pipes[0];
        pc++; // Incrementar o contador de programa
    }

    // Imprime o valor dos registradores
    printf("\nValores finais dos registradores:\n");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%2d] = %d\n", i, R[i]);
    }

    return 0;
}
