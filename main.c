#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256
#define NUM_REGS 32
#define MAX_INSTRUCTIONS 100
#define MEMORY_SIZE 256

// Estrutura para representar uma instrução no pipeline
typedef struct {
    char nomeInstrucao[5]; // Nome da instrução (ex: lw, sw, add, sub, beq)
    int end1, end2, end3; // Registradores e endereço de memória
    int dado1, dado2, dado3; // Dados dos registradores utilizados na instrução
    bool Valido; // Indica se a instrução é válida
} instrucaoFim;

// Estrutura temporária para armazenar os registradores e a memória
typedef struct {
    int memoria[MEMORY_SIZE]; // Memória de dados
    int registradores[NUM_REGS]; // Registradores (R0 a R31), R0 é fixo em 0
} estruturaTemporaria;

// Vetor para armazenar as instruções lidas do arquivo
char txtInstrucoes[MAX_INSTRUCTIONS][MAX_LENGTH];
instrucaoFim instrucoes[MAX_INSTRUCTIONS]; // Array de instruções no pipeline
int numInt = 0; // Número de instruções lidas
int pc = 0; // Contador de programa
estruturaTemporaria temp; // Instância da estrutura temporária

// Função para ler o arquivo com as instruções
void reader() {
    char *filename = "programa.txt"; // Nome do arquivo com as instruções
    FILE *fp = fopen(filename, "r"); // Abre o arquivo para leitura

    if (fp == NULL) { // Verifica se o arquivo foi aberto corretamente
        printf("Error: could not open file %s\n", filename);
        return;
    }

    // Lê as instruções do arquivo e armazena no vetor de instruções
    while (fgets(txtInstrucoes[numInt], MAX_LENGTH, fp) && numInt < MAX_INSTRUCTIONS) {
        // Trata instruções 'noop' e 'halt' separadamente
        if (strncmp(txtInstrucoes[numInt], "noop", 4) == 0 || strncmp(txtInstrucoes[numInt], "halt", 4) == 0) {
            sscanf(txtInstrucoes[numInt], "%s", instrucoes[numInt].nomeInstrucao); // Lê o nome da instrução
            instrucoes[numInt].end1 = instrucoes[numInt].end2 = instrucoes[numInt].end3 = 0; // Zera os endereços
            instrucoes[numInt].Valido = true; // Marca a instrução como válida
            numInt++; // Incrementa o contador de instruções
        } else {
            sscanf(txtInstrucoes[numInt], "%s %d %d %d",
                   instrucoes[numInt].nomeInstrucao,
                   &instrucoes[numInt].end1,
                   &instrucoes[numInt].end2,
                   &instrucoes[numInt].end3); // Lê a instrução e seus operandos
            instrucoes[numInt].Valido = true; // Marca a instrução como válida
            numInt++; // Incrementa o contador de instruções
        }
    }

    fclose(fp); // Fecha o arquivo após a leitura
}

// Estágio 1: Busca a instrução do contador de programa
instrucaoFim fetch(int pc) {
    // Verifica se o contador de programa está dentro dos limites das instruções lidas
    if (pc < numInt) {
        return instrucoes[pc]; // Retorna a instrução correspondente
    }
    return (instrucaoFim){0}; // Retorna uma instrução vazia se fora do limite
}

// Estágio 2: Decodificação da instrução
void decode(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0) return; // Verifica se a instrução é vazia
    if (!instr->Valido) return; // Verifica se a instrução é válida

    // Carrega dados dos registradores conforme necessário
    if (strcmp(instr->nomeInstrucao, "lw") == 0 || strcmp(instr->nomeInstrucao, "sw") == 0) {
        instr->dado1 = instr->end1; // Para lw, end1 é o registrador de destino
        instr->dado2 = temp.registradores[instr->end1]; // Para sw, carrega o valor do registrador
        instr->dado3 = instr->end3; // Endereço de memória
    } else {
        instr->dado1 = temp.registradores[instr->end1]; // Carrega dados do registrador
        instr->dado2 = temp.registradores[instr->end2]; // Carrega dados do segundo registrador
        instr->dado3 = instr->end3; // Carrega o endereço (se aplicável)
    }

    // Exibe informações sobre a instrução sendo decodificada
    printf("--------------------------------------------------\n");
    printf("Decodificando: %-4s | Dado1: %-3d | Dado2: %-3d | Dado3: %-3d\n",
           instr->nomeInstrucao, instr->dado1, instr->dado2, instr->dado3);
}

// Funções específicas para execução de cada tipo de instrução
int execute_add(instrucaoFim *instr) {
    return instr->dado2 + instr->dado3; // Soma os dados dos registradores
}

int execute_sub(instrucaoFim *instr) {
    return instr->dado2 - instr->dado3; // Subtrai os dados dos registradores
}

int execute_beq(instrucaoFim *instr) {
    return (instr->dado1 == instr->dado2) ? instr->end3 : -1; // Retorna o novo PC se a condição for verdadeira
}

// Executa a instrução 'lw' (Load Word)
void execute_lw(instrucaoFim *instr) {
    temp.registradores[instr->end1] = temp.memoria[instr->end3]; // Carrega valor da memória para o registrador
    printf("LW: Carregando R[%d] = %d do endereço %d\n", instr->end1, temp.registradores[instr->end1], instr->end3);
}

// Executa a instrução 'sw' (Store Word)
void execute_sw(instrucaoFim *instr) {
    temp.memoria[instr->end3] = instr->dado2; // Armazena o valor do registrador na memória
    printf("SW: Armazenando R[%d] = %d no endereço %d\n", instr->end1, instr->dado2, instr->end3);
}

// Estágio 3: Execução da instrução
void execute(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0) return; // Verifica se a instrução é vazia

    if (strcmp(instr->nomeInstrucao, "add") == 0) {
        int result = execute_add(instr);
        temp.registradores[instr->end1] = result; // Armazena o resultado no registrador de destino
    } else if (strcmp(instr->nomeInstrucao, "sub") == 0) {
        int result = execute_sub(instr);
        temp.registradores[instr->end1] = result; // Armazena o resultado no registrador de destino
    } else if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        execute_lw(instr); // Executa a instrução de Load Word
    } else if (strcmp(instr->nomeInstrucao, "sw") == 0) {
        execute_sw(instr); // Executa a instrução de Store Word
    }
}

// Função de escrita do resultado no registrador
void write_back(instrucaoFim *instr) {
    if (instr->nomeInstrucao[0] == 0) return; // Verifica se a instrução é vazia

    // Escreve o resultado do registrador se a instrução for de adição ou subtração
    if (strcmp(instr->nomeInstrucao, "add") == 0 || strcmp(instr->nomeInstrucao, "sub") == 0) {
        printf("Escrevendo R[%d] = %d\n", instr->end1, temp.registradores[instr->end1]);
    }
}

// Função de acesso à memória
void memory_access(instrucaoFim *instr) {
    // Não necessário no momento, pois 'lw' e 'sw' são tratados na execução
}

// Loop principal do simulador
int main() {
    instrucaoFim pipes[4] = {0}; // Pipeline com 4 estágios
    memset(temp.memoria, 0, sizeof(temp.memoria)); // Limpa a memória
    memset(temp.registradores, 0, sizeof(temp.registradores)); // Limpa os registradores

    // Ler o arquivo e salvar as instruções no vetor
    reader();

    // Loop do pipeline
    while (pc < numInt || pipes[3].nomeInstrucao[0] != 0 || pipes[2].nomeInstrucao[0] != 0 ||
           pipes[1].nomeInstrucao[0] != 0 || pipes[0].nomeInstrucao[0] != 0) {
        // Estágio 5: Escrita do resultado
        write_back(&pipes[3]);

        // Estágio 4: Acesso à memória
        memory_access(&pipes[3]);

        // Estágio 3: Execução
        execute(&pipes[2]);

        // Estágio 2: Decodificação
        decode(&pipes[1]);

        // Estágio 1: Busca a próxima instrução
        pipes[0] = fetch(pc++); // Busca a próxima instrução e incrementa o PC

        // Atualiza o pipeline, movendo as instruções para o próximo estágio
        pipes[3] = pipes[2]; // Passa a instrução do estágio 3 para o 4
        pipes[2] = pipes[1]; // Passa a instrução do estágio 2 para o 3
        pipes[1] = pipes[0]; // Passa a instrução do estágio 1 para o 2
    }

    // Exibe os resultados finais
    printf("Resultados finais:\n");
    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%d] = %d\n", i, temp.registradores[i]);
    }
    return 0; // Finaliza o programa
}
