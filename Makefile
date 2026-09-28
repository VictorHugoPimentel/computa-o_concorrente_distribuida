CC       = gcc
CFLAGS   = -O2 -std=c11 -Wall -Wextra

BINS     = gerar_entrada merge_sort_seq merge_sort_omp merge_sort_pthreads
TAMANHOS = 1000000 5000000 10000000 20000000 40000000
SEED     = 12345

all: $(BINS)

gerar_entrada: gerar_entrada.c
	$(CC) $(CFLAGS) $< -o $@

merge_sort_seq: merge_sort_seq.c
	$(CC) $(CFLAGS) $< -o $@

merge_sort_omp: merge_sort_omp.c comum.h merge_base.h
	$(CC) $(CFLAGS) -fopenmp $< -o $@

merge_sort_pthreads: merge_sort_pthreads.c comum.h merge_base.h
	$(CC) $(CFLAGS) -pthread $< -o $@

entradas: gerar_entrada
	mkdir -p entradas
	for n in $(TAMANHOS); do ./gerar_entrada $$n $(SEED) entradas/entrada_$$n.bin; done

teste: all
	./testar_correcao.sh

clean:
	rm -f $(BINS)
	rm -rf teste

.PHONY: all entradas teste clean
