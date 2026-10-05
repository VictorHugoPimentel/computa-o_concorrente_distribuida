# Parte 1 — Versão Sequencial e Geração das Entradas

## Trabalho de Programação Concorrente e Distribuída

### Tema do grupo

**Como o tamanho da entrada e o número de threads influenciam o consumo de memória, o desempenho e a eficiência energética de algoritmos de ordenação paralelos?**

### Hipótese do grupo

Espera-se que o aumento do tamanho da entrada eleve o consumo de memória e o tempo de execução.  
Também se espera que o aumento do número de threads reduza o tempo de execução até determinado limite, a partir do qual o overhead de paralelização e a competição pelo acesso à memória reduzam os ganhos de desempenho.

---

# 1. Responsabilidade desta parte

Esta etapa corresponde à preparação da **baseline sequencial** do experimento.

As responsabilidades desta parte foram:

- implementar o algoritmo de ordenação sequencial;
- implementar a geração das entradas;
- garantir que as entradas possam ser reproduzidas;
- validar a correção da ordenação;
- realizar experimentos-piloto;
- selecionar tamanhos adequados de entrada;
- registrar o ambiente de execução;
- preparar o código e as entradas para serem utilizados posteriormente pela versão paralela.

O algoritmo escolhido foi o **Merge Sort**.

---

# 2. Por que o Merge Sort foi escolhido

O Merge Sort possui complexidade temporal:

\[
O(n \log n)
\]

e divide o problema recursivamente em duas partes:

```text
                vetor
                  |
          ----------------
          |              |
       esquerda         direita
          |              |
      merge_sort      merge_sort
          \              /
           ------ merge ------
                  |
            vetor ordenado
```

Essa característica é especialmente interessante para este trabalho porque as duas chamadas recursivas que ordenam as metades do vetor podem posteriormente ser executadas de forma paralela.

Assim, a versão paralela poderá manter a mesma estrutura algorítmica da versão sequencial, facilitando uma comparação mais justa.

---

# 3. Estrutura do projeto

A estrutura utilizada foi:

```text
projeto/
│
├── gerar_entrada.c
├── merge_sort_seq.c
├── README.md
│
└── entradas/
    ├── entrada_1000000.bin
    ├── entrada_5000000.bin
    ├── entrada_10000000.bin
    ├── entrada_20000000.bin
    └── entrada_40000000.bin
```

---

# 4. Geração das entradas

Foi criado um programa separado, `gerar_entrada.c`, responsável pela geração dos vetores utilizados nos experimentos.

Os elementos são números inteiros de 32 bits:

```c
int32_t
```

Cada elemento ocupa:

```text
4 bytes
```

Foi utilizada uma **seed fixa**:

```text
12345
```

O objetivo da seed fixa é permitir que os mesmos dados sejam recriados em qualquer máquina.

Isso é importante porque a versão sequencial e a versão paralela deverão receber **exatamente as mesmas entradas**.

Dessa forma, ao comparar o tempo de execução, a diferença observada estará relacionada à estratégia de execução e não a conjuntos de dados diferentes.

---

# 5. Entradas selecionadas

Após os experimentos-piloto, foram selecionados os seguintes tamanhos:

| Entrada | Número de elementos |
|---|---:|
| E1 | 1.000.000 |
| E2 | 5.000.000 |
| E3 | 10.000.000 |
| E4 | 20.000.000 |
| E5 | 40.000.000 |

Esses valores produzem execuções suficientemente rápidas para permitir repetições, mas também possuem entradas grandes o bastante para que diferenças de desempenho entre a versão sequencial e as futuras versões paralelas possam ser observadas.

---

# 6. Compilação

O gerador de entradas pode ser compilado com:

```bash
gcc -O2 -std=c11 -Wall -Wextra gerar_entrada.c -o gerar_entrada
```

O Merge Sort sequencial foi compilado com:

```bash
gcc -O2 -std=c11 -Wall -Wextra merge_sort_seq.c -o merge_sort_seq
```

As mesmas opções de otimização deverão ser mantidas nas comparações posteriores sempre que possível.

## Flags utilizadas

```text
-O2
```

Ativa otimizações do compilador.

```text
-std=c11
```

Utiliza o padrão C11.

```text
-Wall -Wextra
```

Ativam avisos adicionais do compilador.

---

# 7. Geração dos arquivos de entrada

Primeiro é necessário criar a pasta:

```bash
mkdir -p entradas
```

Depois, as entradas podem ser recriadas com:

```bash
./gerar_entrada 1000000 12345 entradas/entrada_1000000.bin
./gerar_entrada 5000000 12345 entradas/entrada_5000000.bin
./gerar_entrada 10000000 12345 entradas/entrada_10000000.bin
./gerar_entrada 20000000 12345 entradas/entrada_20000000.bin
./gerar_entrada 40000000 12345 entradas/entrada_40000000.bin
```

Como a seed é sempre a mesma, os mesmos dados podem ser gerados novamente.

---

# 8. Execução do Merge Sort sequencial

Exemplo:

```bash
./merge_sort_seq entradas/entrada_10000000.bin
```

A saída possui o seguinte formato:

```text
--- RESULTADO ---
Elementos: 10000000
Tempo: 3.487677 segundos
Ordenacao correta: SIM
CSV;10000000;3.487676703;SIM
```

O programa informa:

- quantidade de elementos;
- tempo gasto na ordenação;
- validação do resultado;
- uma linha em formato simples para facilitar a coleta automática posterior.

---

# 9. Critério utilizado para medir o tempo

O tempo de leitura do arquivo **não faz parte do tempo do algoritmo**.

Primeiro os dados são carregados para a memória.

Somente depois o cronômetro é iniciado:

```c
clock_gettime(CLOCK_MONOTONIC, &inicio);

merge_sort(
    vetor,
    auxiliar,
    0,
    tamanho
);

clock_gettime(CLOCK_MONOTONIC, &fim);
```

Portanto, a medição representa aproximadamente:

\[
T_{ordenação}
\]

e não:

\[
T_{leitura\ do\ arquivo} + T_{ordenação}
\]

Essa decisão é importante porque o objetivo do estudo é comparar o desempenho das estratégias de ordenação, e não avaliar a velocidade do armazenamento.

A versão paralela deverá utilizar o mesmo critério.

---

# 10. Validação da ordenação

Após o Merge Sort, o vetor é percorrido para verificar se todos os elementos estão ordenados.

O teste verifica se:

\[
vetor[i-1] \leq vetor[i]
\]

para todas as posições.

A função retorna sucesso apenas quando todo o vetor está em ordem não decrescente.

Nos experimentos-piloto realizados, todas as entradas apresentaram:

```text
Ordenacao correta: SIM
```

---

# 11. Resultados preliminares

Foram executados testes preliminares para determinar tamanhos adequados de entrada.

| Número de elementos | Tempo sequencial | Resultado |
|---:|---:|:---:|
| 1.000.000 | 0,182753 s | Correto |
| 5.000.000 | 1,179900 s | Correto |
| 10.000.000 | 3,487677 s | Correto |
| 20.000.000 | 5,066288 s | Correto |
| 40.000.000 | 8,567380 s | Correto |

Esses valores são **resultados de experimento-piloto** e não devem ser tratados ainda como os resultados estatísticos finais do trabalho.

Nos experimentos finais, cada configuração deverá ser executada várias vezes para reduzir a influência de:

- escalonamento do sistema operacional;
- processos em segundo plano;
- variação de frequência da CPU;
- caches;
- interferências do ambiente de execução;
- outras fontes de variabilidade.

---

# 12. Observação sobre as 10 execuções

Os testes desta etapa tiveram como finalidade:

1. validar o algoritmo;
2. confirmar a correção da implementação;
3. verificar se os tamanhos de entrada eram adequados;
4. estimar a duração dos experimentos futuros.

Nos experimentos finais, cada configuração foi executada 10 vezes, permitindo posteriormente calcular, por exemplo:

- média;
- mediana;
- desvio padrão;
- mínimo;
- máximo.

---

# 13. Ambiente experimental preliminar

## Processador

```text
AMD Ryzen 7 5700X 8-Core Processor
```

## Arquitetura

```text
x86_64
```

## CPUs lógicas disponíveis

```text
16
```

## Núcleos reportados

```text
8
```

## Threads por núcleo

```text
2
```

## Socket

```text
1
```

## Cache

```text
L1d: 256 KiB (8 instâncias)
L1i: 256 KiB (8 instâncias)
L2: 4 MiB (8 instâncias)
L3: 32 MiB (1 instância)
```

## Memória RAM

O computador possui 32 GB de memória RAM física instalada.

Como os experimentos foram executados através do WSL 2, o ambiente Linux utilizado nos testes reportou:

```text
15 GiB
```

No momento da medição:

```text
Usada:       aproximadamente 617 MiB
Livre:       aproximadamente 14 GiB
Disponível:  aproximadamente 14 GiB
```

## Swap

```text
Total:       4,0 GiB
Usada:       0 B
Livre:       4,0 GiB
```

## Ambiente de virtualização

O programa foi executado no Windows 11 através do WSL 2.

O ambiente Linux utilizado foi:

```text
Linux 6.6.87.2-microsoft-standard-WSL2
```

O sistema reportou:

```text
Hypervisor vendor: Microsoft
Virtualization type: full
```

Essa informação deve ser registrada porque o uso do WSL 2 e de virtualização pode introduzir alguma variabilidade nos resultados.

Para os experimentos finais, foi utilizado sempre o mesmo ambiente.

---

# 14. Compilador

Foi utilizado:

```text
GCC 15.2.0
```

Identificação completa:

```text
gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0
```

Para a versão sequencial:

```bash
gcc -O2 -std=c11 -Wall -Wextra merge_sort_seq.c -o merge_sort_seq
```

Para a versão OpenMP:

```bash
gcc -O2 -std=c11 -Wall -Wextra -fopenmp merge_sort_omp.c -o merge_sort_omp
```

Para a versão Pthreads:

```bash
gcc -O2 -std=c11 -Wall -Wextra -pthread merge_sort_pthreads.c -o merge_sort_pthreads
```

---

# 15. Consumo teórico de memória

A implementação utiliza principalmente dois vetores:

```text
vetor original
+
vetor auxiliar
```

Cada elemento possui:

```text
4 bytes
```

Como os dois vetores possuem o mesmo número de elementos, a memória aproximada utilizada somente por essas duas estruturas é:

\[
Memória \approx n \times 4 \times 2
\]

ou:

\[
Memória \approx 8n \text{ bytes}
\]

## Estimativa

| Elementos | Memória aproximada dos dois vetores |
|---:|---:|
| 1.000.000 | 8 MB |
| 5.000.000 | 40 MB |
| 10.000.000 | 80 MB |
| 20.000.000 | 160 MB |
| 40.000.000 | 320 MB |

Para 40 milhões de elementos:

\[
40.000.000 \times 4 \times 2
=
320.000.000 \text{ bytes}
\]

Isso corresponde a aproximadamente:

```text
305 MiB
```

A memória realmente consumida pelo processo poderá ser um pouco maior devido a:

- stack;
- código executável;
- bibliotecas;
- estruturas internas do runtime;
- alocadores de memória;
- estruturas adicionais.

Nos experimentos finais, o pico real de memória foi coletado através do `/usr/bin/time -v`.

---

# 16. Relação com a pergunta de pesquisa

A versão sequencial funciona como a **baseline** do trabalho.

Ela fornecerá o tempo de referência:

\[
T_s
\]

Esse tempo será posteriormente comparado com o tempo da versão paralela:

\[
T_p
\]

permitindo calcular o speedup:

\[
S = \frac{T_s}{T_p}
\]

Por exemplo, se a versão sequencial levar:

```text
8 segundos
```

e a versão paralela levar:

```text
2 segundos
```

então:

\[
S = \frac{8}{2} = 4
\]

Ou seja, a versão paralela seria quatro vezes mais rápida naquela configuração.

---

# 17. Eficiência paralela futura

Quando os testes com múltiplas threads forem realizados, também será possível calcular:

\[
E = \frac{S}{p}
\]

onde:

- \(E\) = eficiência paralela;
- \(S\) = speedup;
- \(p\) = número de threads.

Por exemplo:

```text
Speedup = 3,2
Threads = 4
```

então:

\[
E = \frac{3,2}{4} = 0,8
\]

ou:

```text
80%
```

Essas métricas serão calculadas posteriormente a partir dos dados coletados nos experimentos.

---

# 18. Configurações de threads

O ambiente utilizado reporta:

```text
8 núcleos físicos
16 CPUs lógicas
```

Nos experimentos das versões OpenMP e Pthreads foram utilizadas:

```text
1 thread
2 threads
4 threads
6 threads
8 threads
10 threads
12 threads
```

Cada configuração foi executada 10 vezes para cada tamanho de entrada.

Essas configurações permitem comparar o comportamento do algoritmo utilizando diferentes níveis de paralelismo, incluindo quantidades de threads superiores ao número de núcleos físicos disponíveis.

O objetivo é observar a partir de qual ponto o aumento de threads deixa de produzir ganho significativo.

---

# 19. O que deve ser mantido na versão paralela

Para garantir uma comparação válida, a versão paralela deverá:

1. utilizar os mesmos arquivos de entrada;
2. utilizar os mesmos tipos de dados;
3. utilizar o mesmo algoritmo base;
4. excluir o tempo de leitura do arquivo da medição;
5. validar a ordenação após a execução;
6. utilizar opções de compilação equivalentes;
7. executar no mesmo ambiente experimental sempre que possível.

---

# 20. Arquivos que devem ser entregues ao responsável pela versão paralela

O responsável pela implementação paralela deverá receber:

```text
gerar_entrada.c
merge_sort_seq.c
README.md
```

e poderá recriar os arquivos de entrada utilizando os comandos:

```bash
./gerar_entrada 1000000 12345 entradas/entrada_1000000.bin
./gerar_entrada 5000000 12345 entradas/entrada_5000000.bin
./gerar_entrada 10000000 12345 entradas/entrada_10000000.bin
./gerar_entrada 20000000 12345 entradas/entrada_20000000.bin
./gerar_entrada 40000000 12345 entradas/entrada_40000000.bin
```

Não é obrigatório armazenar os arquivos binários no repositório, já que eles podem ser recriados deterministicamente.

---

# 21. Texto para a metodologia do artigo

Um texto que poderá ser adaptado posteriormente para o artigo é:

> Como baseline dos experimentos, foi implementada uma versão sequencial do algoritmo Merge Sort na linguagem C. A escolha do Merge Sort permite posteriormente explorar a execução concorrente das ordenações das duas metades do vetor, mantendo a mesma estratégia algorítmica nas versões sequencial e paralela. As entradas são constituídas por vetores de inteiros de 32 bits gerados pseudoaleatoriamente. Para garantir a reprodutibilidade dos experimentos e permitir comparações sob as mesmas condições, foi utilizada uma semente fixa no gerador pseudoaleatório e as entradas foram armazenadas previamente em arquivos binários. O tempo de execução considerado corresponde exclusivamente à etapa de ordenação, excluindo o tempo necessário para leitura dos dados em disco. Após cada execução, o vetor resultante é percorrido para verificar se seus elementos encontram-se em ordem não decrescente.

---

# 22. Resultados do experimento-piloto

Os testes preliminares indicaram crescimento do tempo de execução com o aumento do tamanho da entrada.

A maior entrada testada, com 40 milhões de elementos, levou aproximadamente:

```text
8,57 segundos
```

na execução sequencial.

Esse tempo foi considerado adequado para os experimentos futuros porque:

- é grande o suficiente para reduzir o peso relativo de pequenas variações de tempo;
- é pequeno o suficiente para permitir várias repetições;
- oferece espaço para observar ganhos de desempenho com paralelização.

---

# 23. Execução e coleta dos experimentos

Para automatizar a execução dos testes e a coleta dos resultados foi criado o script:

```text
executar_experimentos.sh
```

Antes de executar os experimentos, os programas devem ser compilados:

```bash
make
```

Em seguida, as entradas utilizadas nos testes podem ser geradas com:

```bash
make entradas
```

Caso necessário, deve-se conceder permissão de execução ao script:

```bash
chmod +x executar_experimentos.sh
```

Para iniciar os experimentos:

```bash
./executar_experimentos.sh
```

O script executa automaticamente as versões sequencial, OpenMP e Pthreads.

São utilizados cinco tamanhos de entrada:

```text
1.000.000
5.000.000
10.000.000
20.000.000
40.000.000 elementos
```

Nas versões paralelas são utilizadas:

```text
1, 2, 4, 6, 8, 10 e 12 threads
```

Cada configuração é executada 10 vezes.

No total são realizadas:

```text
Sequencial:   50 execuções
OpenMP:      350 execuções
Pthreads:    350 execuções
Total:       750 execuções
```

Para cada execução são registrados:

- tempo de execução;
- utilização de CPU;
- pico de memória;
- tamanho da entrada;
- número de threads;
- número da repetição;
- cutoff;
- validação da ordenação.

A utilização de CPU e o pico de memória são obtidos através do comando:

```bash
/usr/bin/time -v
```

Os resultados são armazenados automaticamente em:

```text
resultados/resultados.csv
```

O arquivo possui as seguintes colunas:

```text
versao,tamanho,threads,cutoff,repeticao,tempo_segundos,cpu_percent,memoria_max_kb,correto
```

As informações sobre o ambiente experimental utilizado também foram armazenadas em:

```text
resultados/ambiente.txt
```

Ao final da coleta foram obtidos 750 resultados e todas as execuções apresentaram a ordenação correta.

---

# 24. Situação desta etapa

## Concluído

- [x] Escolha do Merge Sort;
- [x] implementação sequencial;
- [x] gerador de entradas;
- [x] seed fixa para reprodutibilidade;
- [x] arquivos de entrada definidos;
- [x] validação automática da ordenação;
- [x] teste com 1 milhão de elementos;
- [x] teste com 5 milhões de elementos;
- [x] teste com 10 milhões de elementos;
- [x] teste com 20 milhões de elementos;
- [x] teste com 40 milhões de elementos;
- [x] registro do processador;
- [x] registro da memória;
- [x] registro do compilador;
- [x] registro das opções de compilação;
- [x] definição do critério de medição;
- [x] documentação para a versão paralela;
- [x] implementação com OpenMP/Pthreads;
- [x] execução com diferentes números de threads;
- [x] coleta automática de todas as repetições;
- [x] medição experimental de memória;
- [x] medição da utilização de CPU.

## Etapas posteriores do grupo

- [ ] medição ou estimativa de energia;
- [ ] cálculo de speedup;
- [ ] cálculo de eficiência paralela;
- [ ] análise estatística;
- [ ] geração dos gráficos finais;
- [ ] discussão dos resultados.

---

# 25. Resumo da contribuição desta parte

Esta etapa forneceu a baseline necessária para todos os experimentos posteriores.

Foram produzidos:

```text
1. Merge Sort sequencial validado
2. Gerador de entradas reproduzível
3. Cinco tamanhos de entrada definidos
4. Critério consistente para medição do tempo
5. Resultados-piloto
6. Caracterização do ambiente
7. Estimativa teórica de memória
8. Documentação para a versão paralela
```

Além disso, os experimentos posteriores passaram a contar com coleta automatizada de tempo de execução, utilização de CPU e pico de memória para as versões sequencial, OpenMP e Pthreads.

A partir dos dados coletados, as próximas etapas poderão avaliar como a utilização de múltiplas threads influencia:

- tempo de execução;
- speedup;
- eficiência;
- consumo de memória;
- utilização da CPU;
- eficiência energética;
- escalabilidade.