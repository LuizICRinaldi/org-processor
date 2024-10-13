# Organização e Arquitetura de Computadores - Trabalho Grau A

## Integrantes
- Amanda Weimer Monzon
- Cássio Ferreira Braga
- Isabel Humann Petry
- Kelly Natasha de Oliveira Fernandes
- Luís Inácio Costa Rinaldi

## Professor
Lúcio Renê Prade

## Descrição do Projeto
Este trabalho tem como objetivo simular o funcionamento básico de um processador usando um pipeline de 5 estágios. O código recebe um arquivo de texto com instruções escritas em assembly e simula como o processador executaria essas instruções.

Os 5 estágios do pipeline são:

Busca de Instrução: Lê a instrução da memória.

Decodificação: Interpreta a instrução e identifica quais dados são necessários.

Execução: Faz as operações aritméticas (soma, subtração, etc).

Acesso à Memória: Lê ou escreve dados na memória.

Escrita de Resultados: Salva o resultado nos registradores.

## Simulador

Lê o arquivo de instruções e carrega as instruções em assembly.

Simula a execução das instruções dentro dos 5 estágios do pipeline.

Tem um banco de registradores com 32 posições (R0 a R31), onde R0 sempre é 0.

Suporta as seguintes instruções: ADD, SUB, BEQ, LW, SW, NOOP, HALT.

Faz predição de desvio usando a política "não tomado" (o desvio não ocorre por padrão).

## Instruções para uso

O arquivo de texto que o simulador vai ler deve ter as instruções escritas da seguinte forma:

Instruções de operação: <operação> <destino> <operando1> <operando2>

Exemplo: add 1 2 3 (coloca em R1 a soma de R2 e R3)

Desvios condicionais: beq <registrador1> <registrador2> <label>

Exemplo: beq 2 3 10 (se R2 for igual a R3, pula para a linha 10)

Carregar e armazenar dados: lw (load word) e sw (store word)

Exemplo: lw 1 0 10 (carrega o valor da memória no endereço base R0 + 10 para o R1)

## Como compilar e rodar
Para compilar o código e rodar o simulador, basta usar os seguintes comandos no terminal:

gcc -o simulador main.c -I.
./simulador

## O que o compilador faz
Depois de executar o programa, ele vai mostrar os valores finais de todos os registradores. O objetivo principal é verificar como o pipeline funciona e comparar o desempenho com e sem predição de desvio.

