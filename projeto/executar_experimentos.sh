#!/bin/bash

# ============================================================
# EXPERIMENTOS E COLETA DE DADOS
# Projeto de Programacao Concorrente e Distribuida
#
# Algoritmos:
#   - Merge Sort Sequencial
#   - Merge Sort OpenMP
#   - Merge Sort Pthreads
#
# Dados coletados:
#   - Tempo de execucao
#   - Utilizacao de CPU
#   - Pico de memoria
#   - Correcao da ordenacao
# ============================================================

set -u

# ------------------------------------------------------------
# CONFIGURACOES
# ------------------------------------------------------------

TAMANHOS=(
    1000000
    5000000
    10000000
    20000000
    40000000
)

THREADS_LIST=(
    1
    2
    4
    6
    8
    10
    12
)

REPETICOES=10

PASTA_RESULTADOS="resultados"

ARQUIVO="$PASTA_RESULTADOS/resultados.csv"

# ------------------------------------------------------------
# PREPARACAO
# ------------------------------------------------------------

mkdir -p "$PASTA_RESULTADOS"

echo "versao,tamanho,threads,cutoff,repeticao,tempo_segundos,cpu_percent,memoria_max_kb,correto" > "$ARQUIVO"

echo "=================================================="
echo " EXPERIMENTOS DE MERGE SORT"
echo "=================================================="
echo
echo "Tamanhos:"
echo "${TAMANHOS[*]}"
echo
echo "Threads:"
echo "${THREADS_LIST[*]}"
echo
echo "Repeticoes por configuracao: $REPETICOES"
echo
echo "Resultados: $ARQUIVO"
echo
echo "=================================================="
echo


# ============================================================
# FUNCAO: SEQUENCIAL
# ============================================================

executar_sequencial() {

    TAMANHO=$1
    REP=$2

    echo "[SEQ] tamanho=$TAMANHO repeticao=$REP"

    SAIDA_PROGRAMA=$(mktemp)
    SAIDA_TIME=$(mktemp)

    /usr/bin/time -v \
        -o "$SAIDA_TIME" \
        ./merge_sort_seq "entradas/entrada_${TAMANHO}.bin" \
        > "$SAIDA_PROGRAMA"

    LINHA_CSV=$(grep '^CSV;' "$SAIDA_PROGRAMA")

    TEMPO=$(echo "$LINHA_CSV" | cut -d';' -f3)
    CORRETO=$(echo "$LINHA_CSV" | cut -d';' -f4)

    CPU=$(grep "Percent of CPU this job got" "$SAIDA_TIME" \
        | awk '{print $NF}' \
        | tr -d '%')

    MEMORIA=$(grep "Maximum resident set size" "$SAIDA_TIME" \
        | awk '{print $NF}')

    echo "seq,$TAMANHO,1,0,$REP,$TEMPO,$CPU,$MEMORIA,$CORRETO" >> "$ARQUIVO"

    rm "$SAIDA_PROGRAMA" "$SAIDA_TIME"
}


# ============================================================
# FUNCAO: OPENMP
# ============================================================

executar_openmp() {

    TAMANHO=$1
    THREADS=$2
    REP=$3

    echo "[OMP] tamanho=$TAMANHO threads=$THREADS repeticao=$REP"

    SAIDA_PROGRAMA=$(mktemp)
    SAIDA_TIME=$(mktemp)

    /usr/bin/time -v \
        -o "$SAIDA_TIME" \
        ./merge_sort_omp "entradas/entrada_${TAMANHO}.bin" "$THREADS" \
        > "$SAIDA_PROGRAMA"

    LINHA_CSV=$(grep '^CSV;' "$SAIDA_PROGRAMA")

    CUTOFF=$(echo "$LINHA_CSV" | cut -d';' -f5)
    TEMPO=$(echo "$LINHA_CSV" | cut -d';' -f6)
    CORRETO=$(echo "$LINHA_CSV" | cut -d';' -f7)

    CPU=$(grep "Percent of CPU this job got" "$SAIDA_TIME" \
        | awk '{print $NF}' \
        | tr -d '%')

    MEMORIA=$(grep "Maximum resident set size" "$SAIDA_TIME" \
        | awk '{print $NF}')

    echo "omp,$TAMANHO,$THREADS,$CUTOFF,$REP,$TEMPO,$CPU,$MEMORIA,$CORRETO" >> "$ARQUIVO"

    rm "$SAIDA_PROGRAMA" "$SAIDA_TIME"
}


# ============================================================
# FUNCAO: PTHREADS
# ============================================================

executar_pthreads() {

    TAMANHO=$1
    THREADS=$2
    REP=$3

    echo "[PTHREADS] tamanho=$TAMANHO threads=$THREADS repeticao=$REP"

    SAIDA_PROGRAMA=$(mktemp)
    SAIDA_TIME=$(mktemp)

    /usr/bin/time -v \
        -o "$SAIDA_TIME" \
        ./merge_sort_pthreads "entradas/entrada_${TAMANHO}.bin" "$THREADS" \
        > "$SAIDA_PROGRAMA"

    LINHA_CSV=$(grep '^CSV;' "$SAIDA_PROGRAMA")

    CUTOFF=$(echo "$LINHA_CSV" | cut -d';' -f5)
    TEMPO=$(echo "$LINHA_CSV" | cut -d';' -f6)
    CORRETO=$(echo "$LINHA_CSV" | cut -d';' -f7)

    CPU=$(grep "Percent of CPU this job got" "$SAIDA_TIME" \
        | awk '{print $NF}' \
        | tr -d '%')

    MEMORIA=$(grep "Maximum resident set size" "$SAIDA_TIME" \
        | awk '{print $NF}')

    echo "pthreads,$TAMANHO,$THREADS,$CUTOFF,$REP,$TEMPO,$CPU,$MEMORIA,$CORRETO" >> "$ARQUIVO"

    rm "$SAIDA_PROGRAMA" "$SAIDA_TIME"
}


# ============================================================
# EXPERIMENTOS SEQUENCIAIS
# ============================================================

echo
echo "=================================================="
echo " INICIANDO VERSAO SEQUENCIAL"
echo "=================================================="

for TAMANHO in "${TAMANHOS[@]}"
do
    for REP in $(seq 1 "$REPETICOES")
    do
        executar_sequencial "$TAMANHO" "$REP"
    done
done


# ============================================================
# EXPERIMENTOS OPENMP
# ============================================================

echo
echo "=================================================="
echo " INICIANDO OPENMP"
echo "=================================================="

for TAMANHO in "${TAMANHOS[@]}"
do
    for THREADS in "${THREADS_LIST[@]}"
    do
        for REP in $(seq 1 "$REPETICOES")
        do
            executar_openmp "$TAMANHO" "$THREADS" "$REP"
        done
    done
done


# ============================================================
# EXPERIMENTOS PTHREADS
# ============================================================

echo
echo "=================================================="
echo " INICIANDO PTHREADS"
echo "=================================================="

for TAMANHO in "${TAMANHOS[@]}"
do
    for THREADS in "${THREADS_LIST[@]}"
    do
        for REP in $(seq 1 "$REPETICOES")
        do
            executar_pthreads "$TAMANHO" "$THREADS" "$REP"
        done
    done
done


# ============================================================
# FINALIZACAO
# ============================================================

echo
echo "=================================================="
echo " EXPERIMENTOS FINALIZADOS"
echo "=================================================="
echo
echo "Arquivo gerado:"
echo "$ARQUIVO"
echo
echo "Total esperado de execucoes:"
echo "Sequencial: 50"
echo "OpenMP: 350"
echo "Pthreads: 350"
echo "Total: 750"
echo
