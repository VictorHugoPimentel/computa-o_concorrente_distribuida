#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

/*
 * Gerador pseudoaleatório determinístico.
 *
 * Usamos uma seed fixa para que seja possível recriar
 * exatamente a mesma entrada em diferentes experimentos.
 */
static uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    *state = x;

    return x;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr,
                "Uso: %s <quantidade_elementos> <seed> <arquivo_saida>\n",
                argv[0]);

        return EXIT_FAILURE;
    }

    uint64_t quantidade = strtoull(argv[1], NULL, 10);
    uint32_t seed = (uint32_t)strtoul(argv[2], NULL, 10);

    if (quantidade == 0)
    {
        fprintf(stderr, "A quantidade deve ser maior que zero.\n");
        return EXIT_FAILURE;
    }

    if (seed == 0)
    {
        seed = 1;
    }

    FILE *arquivo = fopen(argv[3], "wb");

    if (arquivo == NULL)
    {
        perror("Erro ao criar arquivo");
        return EXIT_FAILURE;
    }

    /*
     * Primeiro valor armazenado no arquivo:
     * quantidade de elementos.
     */
    if (fwrite(&quantidade,
               sizeof(quantidade),
               1,
               arquivo) != 1)
    {
        perror("Erro ao escrever quantidade");
        fclose(arquivo);

        return EXIT_FAILURE;
    }

    const size_t TAMANHO_BUFFER = 1024 * 1024;

    int32_t *buffer =
        malloc(TAMANHO_BUFFER * sizeof(int32_t));

    if (buffer == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria.\n");
        fclose(arquivo);

        return EXIT_FAILURE;
    }

    uint64_t restante = quantidade;

    while (restante > 0)
    {
        size_t quantidade_bloco;

        if (restante > TAMANHO_BUFFER)
        {
            quantidade_bloco = TAMANHO_BUFFER;
        }
        else
        {
            quantidade_bloco = (size_t)restante;
        }

        for (size_t i = 0; i < quantidade_bloco; i++)
        {
            buffer[i] =
                (int32_t)(xorshift32(&seed) & 0x7FFFFFFF);
        }

        size_t escritos = fwrite(
            buffer,
            sizeof(int32_t),
            quantidade_bloco,
            arquivo
        );

        if (escritos != quantidade_bloco)
        {
            perror("Erro ao escrever arquivo");

            free(buffer);
            fclose(arquivo);

            return EXIT_FAILURE;
        }

        restante -= quantidade_bloco;
    }

    free(buffer);
    fclose(arquivo);

    printf(
        "Entrada criada com sucesso.\n"
        "Elementos: %llu\n"
        "Arquivo: %s\n",
        (unsigned long long)quantidade,
        argv[3]
    );

    return EXIT_SUCCESS;
}