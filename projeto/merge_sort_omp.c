#include "comum.h"
#include "merge_base.h"

#include <omp.h>

#define CUTOFF_PADRAO 16384

static void merge_sort_tarefas(
int32_t vetor[],
int32_t auxiliar[],
size_t inicio,
size_t fim,
size_t cutoff
)
{
if (fim - inicio <= cutoff)
{
merge_sort(vetor, auxiliar, inicio, fim);
return;
}

size_t meio = inicio + (fim - inicio) / 2;

#pragma omp task default(none) firstprivate(vetor, auxiliar, inicio, meio, cutoff)
merge_sort_tarefas(vetor, auxiliar, inicio, meio, cutoff);

merge_sort_tarefas(vetor, auxiliar, meio, fim, cutoff);

#pragma omp taskwait

merge(vetor, auxiliar, inicio, meio, fim);


}

int main(int argc, char *argv[])
{
if (argc < 3 || argc > 4)
{
fprintf(stderr, "Uso: %s <arquivo_entrada>  [cutoff]\n", argv[0]);
return EXIT_FAILURE;
}

int threads = atoi(argv[2]);

// Trava de segurança: limita ao máximo de núcleos lógicos do ambiente
if (threads > 12)
{
    printf("Aviso: Numero de threads (%d) excede o limite seguro configurado (12)\n", threads);
    printf("Limitando automaticamente para 12 threads\n");
    threads = 12;
}

long long cutoff_lido = (argc == 4) ? atoll(argv[3]) : CUTOFF_PADRAO;

if (threads < 1 || cutoff_lido < 1)
{
    fprintf(stderr, "threads e cutoff devem ser >= 1.\n");
    return EXIT_FAILURE;
}

size_t cutoff = (size_t)cutoff_lido;

int32_t *vetor;
int32_t *auxiliar;
size_t tamanho;

if (!carregar_entrada(argv[1], &vetor, &auxiliar, &tamanho))
{
    return EXIT_FAILURE;
}

omp_set_dynamic(0);
omp_set_num_threads(threads);

assinatura_t antes = calcular_assinatura(vetor, tamanho);

struct timespec inicio = agora();

#pragma omp parallel default(none) shared(vetor, auxiliar, tamanho, cutoff)
{
    #pragma omp single nowait
    merge_sort_tarefas(vetor, auxiliar, 0, tamanho, cutoff);
}

struct timespec fim = agora();

/* Correto = ordenado E com os mesmos elementos da entrada. */
int correto = esta_ordenado(vetor, tamanho)
           && mesma_assinatura(antes, calcular_assinatura(vetor, tamanho));

imprimir_resultado("omp", tamanho, threads, cutoff, diferenca_tempo(inicio, fim), correto);

free(vetor);
free(auxiliar);

return correto ? EXIT_SUCCESS : EXIT_FAILURE;


}
