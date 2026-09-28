/*
 * comum.h
 *
 * Funções usadas pelas duas versões paralelas (OpenMP e Pthreads):
 *   - leitura da entrada (mesmo formato de gerar_entrada.c);
 *   - cronômetro (mesmo critério da versão sequencial: CLOCK_MONOTONIC,
 *     medindo somente a ordenação, sem a leitura do arquivo);
 *   - validação da correção;
 *   - impressão do resultado no mesmo estilo da versão sequencial.
 */

#ifndef COMUM_H
#define COMUM_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Tempo                                                               */
/* ------------------------------------------------------------------ */

static inline struct timespec agora(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t;
}

static inline double diferenca_tempo(struct timespec inicio, struct timespec fim)
{
    return (double)(fim.tv_sec - inicio.tv_sec)
         + (double)(fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

/* ------------------------------------------------------------------ */
/* Leitura da entrada                                                  */
/* ------------------------------------------------------------------ */

/*
 * Lê o arquivo gerado por gerar_entrada.c:
 * uint64_t com a quantidade, seguido dos int32_t.
 * Aloca também o vetor auxiliar.
 */
static inline int carregar_entrada(
    const char *caminho,
    int32_t **vetor,
    int32_t **auxiliar,
    size_t *tamanho
)
{
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo == NULL)
    {
        perror("Erro ao abrir arquivo");
        return 0;
    }

    uint64_t n;

    if (fread(&n, sizeof(n), 1, arquivo) != 1 || n > SIZE_MAX)
    {
        fprintf(stderr, "Erro ao ler tamanho da entrada.\n");
        fclose(arquivo);
        return 0;
    }

    *tamanho = (size_t)n;
    *vetor = malloc(*tamanho * sizeof(int32_t));
    *auxiliar = malloc(*tamanho * sizeof(int32_t));

    if (*vetor == NULL || *auxiliar == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria para %zu elementos.\n", *tamanho);
        free(*vetor);
        free(*auxiliar);
        fclose(arquivo);
        return 0;
    }

    size_t lidos = fread(*vetor, sizeof(int32_t), *tamanho, arquivo);
    fclose(arquivo);

    if (lidos != *tamanho)
    {
        fprintf(stderr, "Erro: quantidade de elementos incorreta.\n");
        free(*vetor);
        free(*auxiliar);
        return 0;
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* Validação                                                           */
/* ------------------------------------------------------------------ */

/* 1) O vetor está em ordem não decrescente? (igual à versão sequencial) */
static inline int esta_ordenado(const int32_t vetor[], size_t tamanho)
{
    for (size_t i = 1; i < tamanho; i++)
    {
        if (vetor[i - 1] > vetor[i])
        {
            return 0;
        }
    }

    return 1;
}

/*
 * 2) O vetor contém os MESMOS elementos da entrada?
 *
 * Numa versão paralela, uma condição de corrida pode perder ou
 * duplicar elementos e, mesmo assim, deixar o vetor "ordenado".
 * Por isso calculamos uma assinatura independente da ordem
 * (soma, soma dos quadrados e XOR com hash) antes e depois da
 * ordenação, fora do cronômetro.
 */
typedef struct
{
    uint64_t soma;
    uint64_t soma_quadrados;
    uint64_t ou_exclusivo;
} assinatura_t;

static inline assinatura_t calcular_assinatura(const int32_t vetor[], size_t tamanho)
{
    assinatura_t a = { 0, 0, 0 };

    for (size_t i = 0; i < tamanho; i++)
    {
        uint64_t x = (uint64_t)(uint32_t)vetor[i];
        a.soma += x;
        a.soma_quadrados += x * x;
        a.ou_exclusivo ^= x * 0x9E3779B97F4A7C15ULL;
    }

    return a;
}

static inline int mesma_assinatura(assinatura_t a, assinatura_t b)
{
    return a.soma == b.soma
        && a.soma_quadrados == b.soma_quadrados
        && a.ou_exclusivo == b.ou_exclusivo;
}

/* ------------------------------------------------------------------ */
/* Saída                                                               */
/* ------------------------------------------------------------------ */

/*
 * Mesmo estilo da versão sequencial. A linha CSV ganhou as colunas
 * versao, threads e cutoff, necessárias para diferenciar as execuções:
 *
 * CSV;versao;elementos;threads;cutoff;tempo;correto
 */
static inline void imprimir_resultado(
    const char *versao,
    size_t elementos,
    int threads,
    size_t cutoff,
    double tempo,
    int correto
)
{
    printf("\n--- RESULTADO ---\n");
    printf("Versao: %s\n", versao);
    printf("Elementos: %zu\n", elementos);
    printf("Threads: %d\n", threads);
    printf("Tempo: %.6f segundos\n", tempo);
    printf("Ordenacao correta: %s\n", correto ? "SIM" : "NAO");
    printf("CSV;%s;%zu;%d;%zu;%.9f;%s\n",
           versao, elementos, threads, cutoff, tempo,
           correto ? "SIM" : "NAO");
}

#endif
