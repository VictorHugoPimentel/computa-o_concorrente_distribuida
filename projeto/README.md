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

As repetições estatísticas finais deverão ser feitas posteriormente pelo grupo, utilizando o protocolo experimental definido para todas as configurações.

O ideal é executar cada configuração pelo menos 10 vezes e posteriormente calcular, por exemplo:

- média;
- mediana;
- desvio padrão;
- mínimo;
- máximo.

---

# 13. Ambiente experimental

## Processador

```text id="32fmre"
AMD Ryzen 7 5700X 8-Core Processor
```

## Arquitetura

```text id="9miycp"
x86_64
```

## CPUs lógicas disponíveis

```text id="j8qngy"
16
```

## Núcleos físicos

```text id="5te25n"
8
```

## Threads por núcleo

```text id="37bl1d"
2
```

## Socket

```text id="f54mub"
1
```

## Cache

```text id="8gwrrx"
L1d: 256 KiB (8 instâncias)
L1i: 256 KiB (8 instâncias)
L2: 4 MiB (8 instâncias)
L3: 32 MiB (1 instância)
```

## Memória RAM

O computador possui 32 GB de memória RAM física instalada. Entretanto, como os experimentos foram executados através do WSL 2, o ambiente Linux utilizado nos testes reportou:

```text id="h6rb6p"
15 GiB
```

No momento do registro do ambiente:

```text id="y9p1jo"
Usada:       aproximadamente 617 MiB
Livre:       aproximadamente 14 GiB
Disponível:  aproximadamente 14 GiB
```

## Swap

```text id="8m3rnl"
Total:       4,0 GiB
Usada:       0 B
Livre:       4,0 GiB
```

## Sistema operacional e ambiente de virtualização

Os experimentos foram executados em Windows 11 através do WSL 2 (Windows Subsystem for Linux).

O ambiente Linux reportou:

```text id="ojflm7"
Linux 6.6.87.2-microsoft-standard-WSL2
```

O sistema também reportou:

```text id="mfnfpa"
Hypervisor vendor: Microsoft
Virtualization type: full
```

O uso do WSL 2 e de virtualização deve ser considerado na análise, pois pode introduzir alguma variabilidade nas medições. Para manter a consistência dos resultados, todos os experimentos finais foram realizados na mesma máquina e no mesmo ambiente.

---

# 14. Compilador

Foi utilizado:

```text id="w38iyj"
GCC 15.2.0
```

Identificação completa:

```text id="htz1mk"
gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0
```

Para a versão sequencial:

```bash id="yzcy5n"
gcc -O2 -std=c11 -Wall -Wextra merge_sort_seq.c -o merge_sort_seq
```

Para a versão OpenMP:

```bash id="wd2n9x"
gcc -O2 -std=c11 -Wall -Wextra -fopenmp merge_sort_omp.c -o merge_sort_omp
```

Para a versão Pthreads:

```bash id="jnzdg0"
gcc -O2 -std=c11 -Wall -Wextra -pthread merge_sort_pthreads.c -o merge_sort_pthreads
```

O ambiente experimental foi registrado em `resultados/ambiente.txt` antes da execução dos testes.

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

A medição real da memória será realizada posteriormente durante o protocolo experimental.

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

Essas métricas não são calculadas nesta etapa, pois dependem da versão paralela.

---

# 18. Configurações futuras de threads

O ambiente utilizado reporta:

```text
6 núcleos
12 CPUs lógicas
```

Por isso, uma sugestão para os experimentos paralelos é utilizar:

```text
1 thread
2 threads
4 threads
6 threads
8 threads
12 threads
```

A utilização de 6 e 12 threads é interessante porque permite comparar:

- execução abaixo do número de núcleos;
- execução utilizando todos os núcleos reportados;
- execução utilizando threads lógicas adicionais.

O objetivo será observar a partir de qual ponto o aumento de threads deixa de produzir ganho significativo.

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

# 23. Situação desta etapa

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
- [x] documentação para a versão paralela.

## Etapas posteriores do grupo

As etapas abaixo não fazem parte desta implementação sequencial:

- [ ] implementação com threads/OpenMP/Pthreads;
- [ ] execução com diferentes números de threads;
- [ ] coleta automática de todas as repetições;
- [ ] medição experimental de memória;
- [ ] medição ou estimativa de energia;
- [ ] cálculo de speedup;
- [ ] cálculo de eficiência paralela;
- [ ] análise estatística;
- [ ] geração dos gráficos finais;
- [ ] discussão dos resultados.

---

# 24. Resumo da contribuição desta parte

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
8. Documentação para a futura versão paralela
```

A partir desta baseline, as próximas etapas poderão avaliar como a utilização de múltiplas threads influencia:

- tempo de execução;
- speedup;
- eficiência;
- consumo de memória;
- utilização da CPU;
- eficiência energética;
- escalabilidade.

---

# Parte 2 — Versões Paralelas (OpenMP e Pthreads)

---

# 25. Responsabilidade desta parte

Esta etapa corresponde à implementação das **versões paralelas** do Merge Sort em memória compartilhada.

As responsabilidades desta parte foram:

- implementar a versão paralela com OpenMP;
- implementar a versão paralela com Pthreads;
- definir a estratégia de divisão do trabalho entre as threads;
- identificar regiões críticas e necessidades de sincronização;
- verificar a correção dos resultados;
- realizar experimentos preliminares com diferentes quantidades de threads;
- registrar problemas e decisões de implementação.

A versão sequencial (`merge_sort_seq.c`) e o gerador de entradas (`gerar_entrada.c`) **não foram alterados**.

---

# 26. Arquivos adicionados

```text
projeto/
│
├── gerar_entrada.c          (Parte 1)
├── merge_sort_seq.c         (Parte 1)
│
├── merge_sort_omp.c         versão OpenMP (tarefas recursivas)
├── merge_sort_pthreads.c    versão Pthreads (blocos + merge em árvore)
├── merge_base.h             merge() e merge_sort() copiados da versão sequencial
├── comum.h                  leitura da entrada, cronômetro, validação e saída
├── testar_correcao.sh       bateria de testes de correção
├── Makefile                 compila tudo, gera as entradas e roda os testes
└── README.md
```

O arquivo `merge_base.h` contém uma **cópia fiel** das funções `merge()` e `merge_sort()` da versão sequencial.

As duas versões paralelas utilizam exatamente esse mesmo código para intercalar e para ordenar os trechos pequenos.

Dessa forma, qualquer diferença de tempo observada vem da **estratégia de paralelização**, e não de mudanças no algoritmo.

---

# 27. Compilação e execução

Com o `Makefile`:

```bash
make            # compila as 4 versões
make entradas   # recria as 5 entradas com a seed 12345
make teste      # roda a bateria de testes de correção
```

Ou manualmente, com as mesmas flags da Parte 1:

```bash
gcc -O2 -std=c11 -Wall -Wextra -fopenmp merge_sort_omp.c -o merge_sort_omp
gcc -O2 -std=c11 -Wall -Wextra -pthread merge_sort_pthreads.c -o merge_sort_pthreads
```

## Flags adicionais

```text
-fopenmp
```

Ativa o suporte ao OpenMP.

```text
-pthread
```

Ativa o suporte à biblioteca Pthreads.

## Execução

```bash
./merge_sort_omp      entradas/entrada_10000000.bin <threads> [cutoff]
./merge_sort_pthreads entradas/entrada_10000000.bin <threads>
```

O `cutoff` é opcional (padrão: 16384).

## Formato da saída

```text
--- RESULTADO ---
Versao: omp
Elementos: 10000000
Threads: 2
Tempo: 1.124594 segundos
Ordenacao correta: SIM
CSV;omp;10000000;2;16384;1.124593539;SIM
```

A linha CSV das versões paralelas possui o formato:

```text
CSV;versao;elementos;threads;cutoff;tempo;correto
```

As colunas `versao`, `threads` e `cutoff` foram acrescentadas para identificar cada configuração. A versão Pthreads registra `cutoff = 0`, pois não utiliza esse parâmetro.

A versão sequencial mantém o formato original:

```text
CSV;elementos;tempo;correto
```

---

# 28. Estratégias de paralelização

Foram implementadas **duas estratégias diferentes de divisão do trabalho**.

Assim, além do número de threads, o grupo pode comparar também a **forma de paralelizar**.

## 28.1 OpenMP — tarefas recursivas

A versão OpenMP mantém a mesma recursão da versão sequencial.

Em cada nível:

1. a ordenação da metade esquerda vira uma tarefa (`omp task`);
2. a própria thread segue ordenando a metade direita;
3. após um `taskwait`, as duas metades são intercaladas.

```text
                     vetor
             task /          \  própria thread
          esquerda           direita
          task / \           task / \
            ...                ...
                                        até CUTOFF elementos:
                                        merge_sort sequencial
```

Trechos com até `cutoff` elementos **não geram novas tarefas** e são ordenados com o `merge_sort` sequencial original.

O `cutoff` controla, portanto, a **granularidade** das tarefas:

- cutoff pequeno: mais paralelismo, porém maior custo de criação e escalonamento de tarefas;
- cutoff grande: menor custo, porém podem faltar tarefas para ocupar todas as threads.

A divisão do trabalho é **dinâmica**: o runtime do OpenMP entrega as tarefas às threads que estiverem livres, realizando o balanceamento de carga automaticamente.

## 28.2 Pthreads — blocos + merge em árvore

A versão Pthreads utiliza uma decomposição **estática** dos dados, em duas fases.

**Fase 1:** o vetor é dividido em `p` blocos contíguos de tamanho aproximado `n/p`, e cada thread ordena o seu bloco com o `merge_sort` sequencial.

**Fase 2:** os blocos são intercalados em árvore, em aproximadamente log₂(p) rodadas.

Exemplo com 4 threads:

```text
fase 1:        [ T0 ] [ T1 ] [ T2 ] [ T3 ]    4 threads ordenando
rodada s = 1:  [ T0 + T1 ]   [ T2 + T3 ]      2 threads intercalando
rodada s = 2:  [       T0 + T2        ]       1 thread intercalando
```

Características:

- as threads são criadas **uma única vez**;
- a thread principal também trabalha, atuando como T0;
- entre as rodadas existe uma **barreira** (`pthread_barrier_t`);
- funciona com qualquer número de threads, não apenas potências de 2: um grupo sem par passa para a rodada seguinte.

---

# 29. Sincronização, regiões críticas e condições de corrida

Nas duas versões, cada tarefa ou thread **escreve apenas no seu próprio intervalo** `[inicio, fim)` do vetor e do vetor auxiliar.

Como os intervalos de tarefas irmãs não se sobrepõem, **não existe região crítica que exija mutex ou lock**.

A única exigência é de **ordem**: um merge só pode começar depois que as duas metades que ele intercala estiverem prontas.

Isso é garantido por:

| Versão | Mecanismo de sincronização |
|---|---|
| OpenMP | `#pragma omp taskwait` |
| Pthreads | `pthread_barrier_wait` |

## Demonstração de condição de corrida

Ao remover o `#pragma omp taskwait`, o programa continua terminando sem erro aparente, porém a validação passa a indicar:

```text
Ordenacao correta: NAO
```

Isso ocorre porque o merge passa a ler metades que ainda estão sendo ordenadas por outra thread.

O teste confirma que a sincronização utilizada é necessária.

## Limite teórico — Lei de Amdahl

Nas duas versões, o **último merge é executado por uma única thread** e percorre todos os `n` elementos.

Essa parcela serial limita o speedup máximo, independentemente do número de threads utilizado.

---

# 30. Verificação da correção

A validação da Parte 1 (vetor em ordem não decrescente) foi mantida e **reforçada**.

As versões paralelas verificam também se o vetor final **contém exatamente os mesmos elementos da entrada**.

Para isso, é calculada uma assinatura independente da ordem dos elementos:

- soma dos elementos;
- soma dos quadrados;
- XOR com hash.

A assinatura é calculada antes e depois da ordenação, **fora do cronômetro**.

Isso detecta o caso em que uma condição de corrida perde ou duplica elementos e, mesmo assim, o vetor parece ordenado.

A execução só é considerada correta quando:

```text
vetor ordenado  E  mesma assinatura da entrada
```

## Bateria de testes

O script `testar_correcao.sh` executa as duas versões com:

| Parâmetro | Valores testados |
|---|---|
| Tamanhos | 1, 2, 3, 7, 100, 1.000, 65.537 e 1.000.003 |
| Threads | 1, 2, 3, 4, 5, 6, 7, 8, 12, 16 e 32 |
| Cutoffs (OpenMP) | 1, 16 e 16.384 |

Esses valores incluem casos-limite, tamanhos ímpares, quantidades de threads que não são potência de 2 e mais threads do que elementos.

Resultado:

```text
Testes: 352 | Falhas: 0
```

---

# 31. Decisões de implementação

| Decisão | Motivo |
|---|---|
| Reaproveitar o mesmo `merge`/`merge_sort` da versão sequencial | Comparação justa: muda apenas a paralelização |
| Cronômetro iniciado após a leitura do arquivo | Mesmo critério da Parte 1 |
| Cronômetro inclui a criação das threads | Esse custo faz parte do overhead da paralelização |
| Thread principal também trabalha nas duas versões | Com `p` threads, exatamente `p` threads executam trabalho |
| `omp_set_dynamic(0)` | Garante que o OpenMP use exatamente o número de threads solicitado |
| Cutoff padrão de 16.384 elementos | Gera tarefas suficientes para balancear a carga (4.096 tarefas-folha para 40M) sem custo excessivo de criação |
| Blocos do Pthreads com diferença máxima de 1 elemento | Divisão estática o mais equilibrada possível |
| Validação por assinatura, além da ordenação | Detectar condições de corrida que perdem ou duplicam elementos |

---

# 32. Problemas encontrados

- **Condição de corrida ao remover a sincronização:** confirmado que, sem o `taskwait`, o resultado fica incorreto.
- **Número de threads que não é potência de 2** no merge em árvore: resolvido deixando o grupo sem par para a rodada seguinte (testado com 3, 5, 6 e 7 threads).
- **Entradas menores que o número de threads** (por exemplo, 3 elementos com 32 threads): algumas threads recebem blocos vazios, e o código trata esse caso corretamente.

---

# 33. Experimento-piloto das versões paralelas

## Google Colab

Ambiente reportado:

```text
Model name:          Intel(R) Xeon(R) CPU @ 2.20GHz
Thread(s) per core:  2
Core(s) per socket:  1
```

Ou seja, **1 núcleo físico com 2 threads lógicas** (hyperthreading).

| Versão | Threads | Tempo (n = 10.000.000) | Speedup |
|---|---:|---:|---:|
| Sequencial | 1 | 1,597 s | 1,00 |
| OpenMP | 2 | 1,125 s | 1,42 |
| Pthreads | 2 | 1,070 s | 1,49 |

O speedup de aproximadamente 1,5× ficou abaixo do ideal de 2×. Isso é consistente com o ambiente: as duas threads lógicas compartilham as mesmas unidades de execução e a mesma cache de um único núcleo físico.

## Ambiente de nuvem com 2 núcleos físicos

Mediana de 3 execuções, Intel Xeon 2,1 GHz (virtualizado):

| n | Versão | 1 thread | 2 threads | 4 threads | Sequencial |
|---:|---|---:|---:|---:|---:|
| 10M | OpenMP | 1,18 s | 0,73 s | 0,73 s | 1,41 s |
| 10M | Pthreads | 1,09 s | 0,62 s | 0,61 s | |
| 40M | OpenMP | 5,63 s | 2,97 s | 3,27 s | 5,75 s |
| 40M | Pthreads | 5,35 s | 2,73 s | 2,61 s | |

Observações:

- com 2 threads, as duas versões ficaram entre 1,9× e 2,3× mais rápidas que a sequencial;
- valores acima de 2× com 2 núcleos não representam ganho real: indicam variação na própria medição da versão sequencial;
- com 4 threads não houve ganho adicional, como esperado com apenas 2 núcleos;
- as versões com 1 thread ficaram mais rápidas que a sequencial mesmo executando o mesmo algoritmo, o que também indica variação do ambiente.

Esses valores são **resultados de experimento-piloto**. Eles servem apenas para confirmar que as versões paralelas funcionam e obtêm ganho de desempenho. Os resultados finais deverão ser obtidos no ambiente experimental definido pelo grupo, com pelo menos 10 repetições por configuração.

---

# 34. Observações para as próximas etapas

## Processador híbrido

O processador utilizado na Parte 1 (i5-1245U) é **híbrido**:

```text
2 núcleos de desempenho (P-cores), com 2 threads cada
8 núcleos de eficiência (E-cores), com 1 thread cada
Total: 10 núcleos físicos e 12 threads
```

O WSL reporta "6 núcleos × 2 threads por núcleo", o que não corresponde ao hardware real.

Isso deve influenciar os resultados acima de 4 threads e é importante para a discussão. Por exemplo:

- na divisão estática do Pthreads, um bloco atribuído a um núcleo de eficiência pode atrasar a barreira;
- na divisão dinâmica do OpenMP, as tarefas podem ser redistribuídas entre as threads livres.

## Configurações sugeridas

```text
Sequencial
1, 2, 4, 6, 8, 10 e 12 threads (OpenMP e Pthreads)
```

## Experimento opcional — granularidade

Variar o cutoff da versão OpenMP, por exemplo:

```text
1024, 4096, 16384, 65536, 262144
```

---

# 35. Texto para a metodologia do artigo

Um texto que poderá ser adaptado posteriormente para o artigo é:

> Foram implementadas duas versões paralelas do Merge Sort em C, ambas reutilizando as mesmas rotinas de ordenação e intercalação da versão sequencial. A versão em OpenMP preserva a estrutura recursiva do algoritmo: em cada nível, a ordenação de uma das metades é delegada a uma tarefa (`omp task`) e a intercalação ocorre após a sincronização com `taskwait`; subvetores com até 16.384 elementos são ordenados sequencialmente, limitando o custo de criação de tarefas. A versão em Pthreads adota uma decomposição estática de dados: o vetor é dividido em *p* blocos contíguos, ordenados de forma independente pelas threads e intercalados em árvore em ⌈log₂ *p*⌉ rodadas separadas por barreiras. Em ambas as versões, cada thread escreve apenas em intervalos disjuntos do vetor, de modo que nenhuma exclusão mútua é necessária; a sincronização limita-se às dependências entre as ordenações das metades e sua intercalação. A intercalação final é executada por uma única thread e percorre todos os elementos, constituindo uma parcela serial que, pela Lei de Amdahl, limita o speedup alcançável. A correção foi verificada comparando, além da ordenação, uma assinatura independente da ordem dos elementos calculada antes e depois da execução.

---

# 36. Situação desta etapa

## Concluído

- [x] implementação com OpenMP;
- [x] implementação com Pthreads;
- [x] definição da estratégia de divisão do trabalho;
- [x] identificação das regiões críticas e da sincronização necessária;
- [x] demonstração de condição de corrida;
- [x] validação reforçada (ordenação + assinatura);
- [x] bateria de testes de correção (352 casos);
- [x] experimentos preliminares com 1, 2 e 4 threads;
- [x] registro das decisões e dos problemas encontrados.

## Etapas posteriores do grupo

- [ ] execução com diferentes números de threads no ambiente final;
- [ ] coleta automática de todas as repetições;
- [ ] medição experimental de memória;
- [ ] medição ou estimativa de energia;
- [ ] cálculo de speedup e eficiência paralela;
- [ ] análise estatística;
- [ ] geração dos gráficos finais;
- [ ] discussão dos resultados.
