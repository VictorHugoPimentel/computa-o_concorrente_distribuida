#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>

static void merge(
    int32_t vetor[],
    int32_t auxiliar[],
    size_t inicio,
    size_t meio,
    size_t fim
)
{
    size_t i = inicio;
    size_t j = meio;
    size_t k = inicio;

    while (i < meio && j < fim)
    {
        if (vetor[i] <= vetor[j])
        {
            auxiliar[k++] = vetor[i++];
        }
        else
        {
            auxiliar[k++] = vetor[j++];
        }
    }

    while (i < meio)
    {
        auxiliar[k++] = vetor[i++];
    }

    while (j < fim)
    {
        auxiliar[k++] = vetor[j++];
    }

    for (size_t pos = inicio; pos < fim; pos++)
    {
        vetor[pos] = auxiliar[pos];
    }
}

static void merge_sort(
    int32_t vetor[],
    int32_t auxiliar[],
    size_t inicio,
    size_t fim
)
{
    if (fim - inicio <= 1)
    {
        return;
    }

    size_t meio = inicio + (fim - inicio) / 2;

    merge_sort(
        vetor,
        auxiliar,
        inicio,
        meio
    );

    merge_sort(
        vetor,
        auxiliar,
        meio,
        fim
    );

    merge(
        vetor,
        auxiliar,
        inicio,
        meio,
        fim
    );
}

static int esta_ordenado(
    const int32_t vetor[],
    size_t tamanho
)
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

static double diferenca_tempo(
    struct timespec inicio,
    struct timespec fim
)
{
    double segundos =
        (double)(fim.tv_sec - inicio.tv_sec);

    double nanossegundos =
        (double)(fim.tv_nsec - inicio.tv_nsec)
        / 1000000000.0;

    return segundos + nanossegundos;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(
            stderr,
            "Uso: %s <arquivo_entrada>\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    FILE *arquivo = fopen(argv[1], "rb");

    if (arquivo == NULL)
    {
        perror("Erro ao abrir arquivo");
        return EXIT_FAILURE;
    }

    uint64_t tamanho_arquivo;

    if (fread(
            &tamanho_arquivo,
            sizeof(tamanho_arquivo),
            1,
            arquivo
        ) != 1)
    {
        fprintf(
            stderr,
            "Erro ao ler tamanho da entrada.\n"
        );

        fclose(arquivo);
        return EXIT_FAILURE;
    }

    if (tamanho_arquivo > SIZE_MAX)
    {
        fprintf(stderr, "Entrada muito grande.\n");

        fclose(arquivo);
        return EXIT_FAILURE;
    }

    size_t tamanho = (size_t)tamanho_arquivo;

    int32_t *vetor =
        malloc(tamanho * sizeof(int32_t));

    int32_t *auxiliar =
        malloc(tamanho * sizeof(int32_t));

    if (vetor == NULL || auxiliar == NULL)
    {
        fprintf(
            stderr,
            "Erro ao alocar memoria para %zu elementos.\n",
            tamanho
        );

        free(vetor);
        free(auxiliar);
        fclose(arquivo);

        return EXIT_FAILURE;
    }

    size_t elementos_lidos =
        fread(
            vetor,
            sizeof(int32_t),
            tamanho,
            arquivo
        );

    fclose(arquivo);

    if (elementos_lidos != tamanho)
    {
        fprintf(
            stderr,
            "Erro: quantidade de elementos incorreta.\n"
        );

        free(vetor);
        free(auxiliar);

        return EXIT_FAILURE;
    }

    struct timespec inicio;
    struct timespec fim;

    /*
     * O cronometro começa somente depois da leitura
     * da entrada.
     *
     * Dessa forma medimos o algoritmo e não o acesso
     * ao disco.
     */
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    merge_sort(
        vetor,
        auxiliar,
        0,
        tamanho
    );

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo =
        diferenca_tempo(inicio, fim);

    int correto =
        esta_ordenado(vetor, tamanho);

    printf("\n--- RESULTADO ---\n");

    printf(
        "Elementos: %zu\n",
        tamanho
    );

    printf(
        "Tempo: %.6f segundos\n",
        tempo
    );

    printf(
        "Ordenacao correta: %s\n",
        correto ? "SIM" : "NAO"
    );

    /*
     * Linha simples para futuramente ser utilizada
     * pelo script de coleta dos experimentos.
     */
    printf(
        "CSV;%zu;%.9f;%s\n",
        tamanho,
        tempo,
        correto ? "SIM" : "NAO"
    );

    free(vetor);
    free(auxiliar);

    return correto
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}