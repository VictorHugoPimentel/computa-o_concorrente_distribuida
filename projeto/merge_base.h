/*
 * merge_base.h
 *
 * Cópia fiel das funções merge() e merge_sort() de
 * merge_sort_seq.c. As versões paralelas usam exatamente
 * este mesmo código para intercalar e para ordenar os
 * trechos pequenos, então qualquer diferença de tempo vem
 * da estratégia de paralelização, e não de mudanças no
 * algoritmo.
 */

#ifndef MERGE_BASE_H
#define MERGE_BASE_H

#include <stddef.h>
#include <stdint.h>

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

    merge_sort(vetor, auxiliar, inicio, meio);
    merge_sort(vetor, auxiliar, meio, fim);
    merge(vetor, auxiliar, inicio, meio, fim);
}

#endif
