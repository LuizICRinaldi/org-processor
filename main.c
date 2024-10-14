#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 256
#define NUM_REGS 32
#define MAX_INSTRUCTIONS 100

// Estrutura que representa uma instrucao no pipeline
typedef struct {
    char nomeInstrucao[5]; // Nome da instrucao (ex: "add", "sub", "beq", etc.)
    int end1, end2, end3; // Enderecos dos registradores ou offset
    int dado1, dado2, dado3; // Valores dos registradores
    int posicao;
    int targetRegister;
    bool Valido; // Indica se a instrucao e valida
} instrucaoFim;

// Armazenamento das instrucoes lidas do arquivo
char txtInstrucoes[MAX_INSTRUCTIONS][255];

// Banco de registradores (R0 a R31)
int R[NUM_REGS] = {0};
instrucaoFim instrucoes[MAX_INSTRUCTIONS]; // Vetor para armazenar instrucoes
int numInt = 0; // Contador de instrucoes
int pc = 0; // Contador de programa
int memory[256] = {0}; // Memoria de dados

char *instList[] = {"noop",
                        "halt",
                        "add",
                        "sub",
                        "lw",
                        "sw",
                        "beq"
};

int memData = 0, ulaResult = 0;

// Funcao para ler o arquivo com as instrucoes
void reader() {
    char *filename = "programa.txt"; // Nome do arquivo contendo as instrucoes
    FILE *fp = fopen(filename, "r"); // Abre o arquivo

    if (fp == NULL) {
        printf("Error: could not open file %s\n", filename); // Mensagem de erro ao abrir o arquivo
        return;
    }

    // Leitura das instrucoes do arquivo
    while (fgets(txtInstrucoes[numInt], MAX_LENGTH, fp) && numInt < MAX_INSTRUCTIONS) {
        // Tratamento de instrucoes "noop" e "halt"
        if (strncmp(txtInstrucoes[numInt], "noop", 4) == 0 || strncmp(txtInstrucoes[numInt], "halt", 4) == 0) {
            sscanf(txtInstrucoes[numInt], "%s", instrucoes[numInt].nomeInstrucao);
            instrucoes[numInt].end1 = 0;
            instrucoes[numInt].end2 = 0;
            instrucoes[numInt].end3 = 0;
            instrucoes[numInt].Valido = false;
            numInt++;
        } else {
            sscanf(txtInstrucoes[numInt], "%d %s %d %d %d",
                   &instrucoes[numInt].posicao,
                   instrucoes[numInt].nomeInstrucao,
                   &instrucoes[numInt].end1,
                   &instrucoes[numInt].end2,
                   &instrucoes[numInt].end3);
            instrucoes[numInt].Valido = true; // Marcar como valida
            numInt++;
        }
    }

    fclose(fp); // Fecha o arquivo
}

// Estagio 1: Busca da instrucao
instrucaoFim *fetch(int pc) {
    if (pc < numInt) {
        return &instrucoes[pc]; // Retorna a instrucao no endereco atual do PC
    }
    return NULL; // Retorna instrucao vazia se o PC estiver fora do alcance
}

// Estagio 2: Decodificacao da instrucao
void decode(instrucaoFim *instr) {
    if(instr == NULL || !instr->Valido) return; // Verifica se a instrucao e valida

    // Le os dados dos registradores
    instr->dado1 = R[instr->end1];
    instr->dado2 = R[instr->end2];
    instr->dado3 = R[instr->end3];

    printf("Decodificando: %s, Dado1: %d, Dado2: %d, Dado3: %d\n",
           instr->nomeInstrucao, instr->dado1, instr->dado2, instr->dado3);
}

// Estagio 3: Execucao da instrucao
void execute(instrucaoFim *instr) {
    if (instr == NULL || !instr->Valido) return; // Verifica se a instrucao e valida

    int result = 0; // Inicializa o resultado

    instr->targetRegister = instr->end3;
    if (strcmp(instr->nomeInstrucao, "add") == 0) {
        result = instr->dado1 + instr->dado2;
    } else if (strcmp(instr->nomeInstrucao, "sub") == 0) {
        result = instr->dado1 - instr->dado2;
    } else if (strcmp(instr->nomeInstrucao, "lw") == 0 || strcmp(instr->nomeInstrucao, "sw") == 0) {
        result = instr->dado1 + instr->end3;
        instr->targetRegister = instr->end2;
    } else if (strcmp(instr->nomeInstrucao, "beq") == 0) {
        result =  (instr->dado1 == instr->dado2) ? 1 : 0;
    } 
    
	printf("Executando: %s, Resultado: %d\n", instr->nomeInstrucao, result);
    ulaResult = result; // Salva o resultado da execucao
}

// Estagio 4: Acesso a memoria
void memory_access(instrucaoFim *instr) {
    if(instr == NULL || instr->Valido == false) return; // Verifica se a instrucao e valida

    if (strcmp(instr->nomeInstrucao, "lw") == 0) {
        memData =  memory[ulaResult];
    } else if (strcmp(instr->nomeInstrucao, "sw") == 0) {
        memory[ulaResult] = instr->dado2;
        memData =  memory[ulaResult];
    } else {
        memData = ulaResult;
    }

    
}

// Estagio 5: Escrita no banco de registradores
void write_back(instrucaoFim *instr) {
    if(instr == NULL) return;
    if(instr->Valido == false) return;

    R[instr->targetRegister] = memData;
    printf("Escrevendo R[%d] = %d\n", instr->targetRegister, memData);
}

int main() {
	// Estruturas para armazenar instrucoes em cada estagio do pipeline
    instrucaoFim *pipes[5] = {NULL};

    //Inicia valores da memoria
    memory[10] = 100;
    memory[11] = 254;

    //Le arquivo do disco e salva em um array de instrucoes
    reader();

    //Entra a cada ciclo de clock
    //Fica iterando enquanto huver alguma instrucao valida, e pc for menor do que a quantidade de instrucoes
    while(pc < numInt || pipes[4] != NULL || pipes[3] != NULL || 
      pipes[2] != NULL || pipes[1] != NULL || pipes[0] != NULL) {

        // Estagios de pipeline
        write_back(pipes[4]); // Estagio 5: WB
        memory_access(pipes[3]); // Estagio 4: MEM
        execute(pipes[2]); // Estagio 3: EX
        decode(pipes[1]); // Estagio 2: ID
        pipes[0] = fetch(pc); // Estagio 1: IF

		// Avanca o pipeline
        pipes[4] = pipes[3];
        pipes[3] = pipes[2];
        pipes[2] = pipes[1];
        pipes[1] = pipes[0];
        pc++;
    }

	// Imprime o valor dos registradores
    for (int i = 0; i < NUM_REGS; i++) {
        printf("R[%d] = %d\n", i, R[i]);
    }

    return 0; // Finaliza o programa
}
