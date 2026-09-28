#define _POSIX_C_SOURCE 200809L
#include "comum.h"
#include "merge_base.h"

#include <pthread.h>

typedef struct
{
    int id;
    int total_threads;
    int32_t *vetor;
    int32_t *auxiliar;
    const size_t *limites; 
    pthread_barrier_t *barreira;
} argumento_t;

static void *trabalhador(void *arg)
{
    argumento_t *a = (argumento_t *)arg;
    int t = a->id;
    int p = a->total_threads;

    merge_sort(a->vetor, a->auxiliar, a->limites[t], a->limites[t + 1]);

    /* Fase 2: merge em árvore. */
    for (int s = 1; s < p; s *= 2)
    {
        pthread_barrier_wait(a->barreira);

        if (t % (2 * s) == 0 && t + s < p)
        {
            int ultimo = (t + 2 * s < p) ? t + 2 * s : p;

            merge(
                a->vetor,
                a->auxiliar,
                a->limites[t],
                a->limites[t + s],
                a->limites[ultimo]
            );
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int p = atoi(argv[2]);

    if (p < 1)
    {
        fprintf(stderr, "threads deve ser >= 1.\n");
        return EXIT_FAILURE;
    }

    int32_t *vetor;
    int32_t *auxiliar;
    size_t tamanho;

    if (!carregar_entrada(argv[1], &vetor, &auxiliar, &tamanho))
    {
        return EXIT_FAILURE;
    }

    size_t *limites = malloc((size_t)(p + 1) * sizeof(size_t));
    pthread_t *ids = malloc((size_t)p * sizeof(pthread_t));
    argumento_t *args = malloc((size_t)p * sizeof(argumento_t));

    if (limites == NULL || ids == NULL || args == NULL)
    {
        fprintf(stderr, "Erro ao alocar estruturas das threads.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i <= p; i++)
    {
        limites[i] = (size_t)((unsigned __int128)tamanho * (unsigned)i / (unsigned)p);
    }

    pthread_barrier_t barreira;

    for (int i = 0; i < p; i++)
    {
        args[i] = (argumento_t){ i, p, vetor, auxiliar, limites, &barreira };
    }

    /* Mesmo critério: começa depois da leitura, inclui criação das threads. */
    assinatura_t antes = calcular_assinatura(vetor, tamanho);

    struct timespec inicio = agora();

    pthread_barrier_init(&barreira, NULL, (unsigned)p);

    for (int i = 1; i < p; i++)
    {
        if (pthread_create(&ids[i], NULL, trabalhador, &args[i]) != 0)
        {
            fprintf(stderr, "Erro ao criar thread %d.\n", i);
            return EXIT_FAILURE;
        }
    }

    trabalhador(&args[0]);

    for (int i = 1; i < p; i++)
    {
        pthread_join(ids[i], NULL);
    }

    pthread_barrier_destroy(&barreira);

    struct timespec fim = agora();

    int correto = esta_ordenado(vetor, tamanho)
               && mesma_assinatura(antes, calcular_assinatura(vetor, tamanho));

    imprimir_resultado("pthreads", tamanho, p, 0, diferenca_tempo(inicio, fim), correto);

    free(limites);
    free(ids);
    free(args);
    free(vetor);
    free(auxiliar);

    return correto ? EXIT_SUCCESS : EXIT_FAILURE;
}
