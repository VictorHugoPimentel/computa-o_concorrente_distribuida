#!/usr/bin/env bash
#
# testar_correcao.sh — verificação da correção das versões paralelas.
#
# Roda as duas versões para vários tamanhos (inclusive casos-limite,
# como 1, 2, 3 e 7 elementos, e tamanhos ímpares), vários números de
# threads (inclusive valores que não são potência de 2 e maiores que
# o número de núcleos) e vários cutoffs do OpenMP.
#
# Cada execução só é considerada correta se o vetor final estiver
# ordenado E tiver os mesmos elementos da entrada.
#
set -u

TAMANHOS="1 2 3 7 100 1000 65537 1000003"
THREADS="1 2 3 4 5 6 7 8 12 16 32"
CUTOFFS="1 16 16384"

mkdir -p teste
total=0
falhas=0

for n in $TAMANHOS; do
    ./gerar_entrada "$n" 12345 "teste/t_$n.bin" > /dev/null
    for p in $THREADS; do
        for c in $CUTOFFS; do
            total=$((total + 1))
            ./merge_sort_omp "teste/t_$n.bin" "$p" "$c" > /dev/null \
                || { echo "FALHA: omp n=$n p=$p cutoff=$c"; falhas=$((falhas + 1)); }
        done
        total=$((total + 1))
        ./merge_sort_pthreads "teste/t_$n.bin" "$p" > /dev/null \
            || { echo "FALHA: pthreads n=$n p=$p"; falhas=$((falhas + 1)); }
    done
done

echo "Testes: $total | Falhas: $falhas"
[ "$falhas" -eq 0 ]
