/*
  Módulo: Priorizar & Montar Torre de Resgate
  Autor: (Exemplo)
  Descrição:
    - Define uma struct Componente { nome, tipo, prioridade }
    - Permite cadastro de até 20 componentes (uso de fgets)
    - Oferece três algoritmos de ordenação:
        * Bubble sort por nome (string)
        * Insertion sort por tipo (string)
        * Selection sort por prioridade (int)
    - Mede número de comparações de cada algoritmo e tempo de execução (clock)
    - Permite busca binária por nome (após ordenar por nome)
    - Interface de menu interativo
    - Código modular e comentado
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_COMPONENTES 20
#define MAX_NOME 30
#define MAX_TIPO 20

typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int prioridade;
} Componente;

/* ---------- Assinaturas de funções ---------- */

/* Ordenadores retornam comparações via ponteiro long*comparacoes */
void bubbleSortNome(Componente arr[], int n, long *comparacoes);
void insertionSortTipo(Componente arr[], int n, long *comparacoes);
void selectionSortPrioridade(Componente arr[], int n, long *comparacoes);

/* Busca binária por nome (apenas após ordenar por nome) */
int buscaBinariaPorNome(Componente arr[], int n, const char chave[], long *comparacoes);

/* Utilitários */
void mostrarComponentes(Componente arr[], int n);
void trim_newline(char *s);
double medirTempo(void (*algoritmo)(Componente*, int, long*), Componente arr[], int n, long *comparacoes);

/* Faz uma cópia do vetor de componentes (útil para comparar algoritmos com os mesmos dados) */
Componente* copiarArray(Componente* origem, int n);

/* ---------- Implementação das funções ---------- */

/* Remove '\n' ao final de string (vinda do fgets) */
void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len == 0) return;
    if (s[len-1] == '\n') s[len-1] = '\0';
}

/* Mostra o vetor de componentes de forma tabulada */
void mostrarComponentes(Componente arr[], int n) {
    printf("\n--- Componentes (%d) ---\n", n);
    printf("%-3s | %-30s | %-20s | %-9s\n", "No", "Nome", "Tipo", "Prioridade");
    printf("----+--------------------------------+----------------------+-----------\n");
    for (int i = 0; i < n; i++) {
        printf("%-3d | %-30s | %-20s | %-9d\n", i, arr[i].nome, arr[i].tipo, arr[i].prioridade);
    }
    printf("------------------------------\n");
}

/* Bubble sort por nome (ordem lexicográfica ascendente)
   Conta comparações de strings (cada strcmp conta como 1 comparação) */
void bubbleSortNome(Componente arr[], int n, long *comparacoes) {
    *comparacoes = 0;
    int trocou;
    for (int i = 0; i < n-1; i++) {
        trocou = 0;
        for (int j = 0; j < n-1-i; j++) {
            (*comparacoes)++;
            if (strcmp(arr[j].nome, arr[j+1].nome) > 0) {
                Componente tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

/* Insertion sort por tipo (ordem lexicográfica ascendente)
   Conta comparações de strings (cada strcmp conta como 1 comparação) */
void insertionSortTipo(Componente arr[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < n; i++) {
        Componente chave = arr[i];
        int j = i - 1;
        /* enquanto j >= 0 e arr[j].tipo > chave.tipo */
        while (j >= 0) {
            (*comparacoes)++;
            if (strcmp(arr[j].tipo, chave.tipo) > 0) {
                arr[j+1] = arr[j];
                j--;
            } else {
                break;
            }
        }
        arr[j+1] = chave;
    }
}

/* Selection sort por prioridade (menor prioridade primeiro)
   Conta comparações inteiras (cada comparação de prioridade conta como 1) */
void selectionSortPrioridade(Componente arr[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n-1; i++) {
        int idxMin = i;
        for (int j = i+1; j < n; j++) {
            (*comparacoes)++;
            if (arr[j].prioridade < arr[idxMin].prioridade) {
                idxMin = j;
            }
        }
        if (idxMin != i) {
            Componente tmp = arr[i];
            arr[i] = arr[idxMin];
            arr[idxMin] = tmp;
        }
    }
}

/* Busca binária por nome (vetor deve estar ordenado por nome).
   Retorna índice se encontrou, -1 caso contrário.
   Conta comparações de strings (cada strcmp conta como 1 comparação) */
int buscaBinariaPorNome(Componente arr[], int n, const char chave[], long *comparacoes) {
    int low = 0, high = n - 1;
    *comparacoes = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        (*comparacoes)++;
        int cmp = strcmp(arr[mid].nome, chave);
        if (cmp == 0) return mid;
        else if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/* Wrapper para medir o tempo de execução do algoritmo (usa clock()).
   Recebe um ponteiro para função de ordenação com assinatura (Componente*, int, long*).
   Retorna tempo em segundos e preenche comparacoes. */
double medirTempo(void (*algoritmo)(Componente*, int, long*), Componente arr[], int n, long *comparacoes) {
    clock_t inicio = clock();
    algoritmo(arr, n, comparacoes);
    clock_t fim = clock();
    return ((double)(fim - inicio)) / CLOCKS_PER_SEC;
}

/* Faz uma cópia do array de componentes */
Componente* copiarArray(Componente* origem, int n) {
    Componente* copia = (Componente*) malloc(sizeof(Componente) * n);
    if (!copia) return NULL;
    memcpy(copia, origem, sizeof(Componente) * n);
    return copia;
}

/* ---------- Função main: interface de menu e fluxo ---------- */
int main() {
    setbuf(stdout, NULL); // para evitar buffering c/ alguns ambientes
    int n = 0;
    printf("=== Montagem: Torre de Resgate (Módulo de Prioridades) ===\n\n");

    /* Entrada do número de componentes (1..MAX_COMPONENTES) */
    do {
        printf("Quantos componentes deseja cadastrar? (1 a %d): ", MAX_COMPONENTES);
        if (scanf("%d", &n) != 1) {
            while (getchar() != '\n'); // limpa stdin
            n = 0;
        }
    } while (n < 1 || n > MAX_COMPONENTES);
    while (getchar() != '\n'); // limpa '\n' residual

    /* Aloca dinamicamente o vetor de componentes */
    Componente *componentes = (Componente*) malloc(sizeof(Componente) * n);
    if (!componentes) {
        printf("Erro: memória insuficiente.\n");
        return 1;
    }

    /* Leitura dos componentes com fgets */
    for (int i = 0; i < n; i++) {
        printf("\nComponente %d:\n", i);
        printf("Nome (max %d chars): ", MAX_NOME-1);
        if (!fgets(componentes[i].nome, sizeof(componentes[i].nome), stdin)) {
            componentes[i].nome[0] = '\0';
        }
        trim_newline(componentes[i].nome);

        printf("Tipo (max %d chars): ", MAX_TIPO-1);
        if (!fgets(componentes[i].tipo, sizeof(componentes[i].tipo), stdin)) {
            componentes[i].tipo[0] = '\0';
        }
        trim_newline(componentes[i].tipo);

        /* prioridade: valida 1..10 */
        int p = 0;
        do {
            printf("Prioridade (1 a 10): ");
            if (scanf("%d", &p) != 1) {
                while (getchar() != '\n');
                p = 0;
            }
        } while (p < 1 || p > 10);
        componentes[i].prioridade = p;
        while (getchar() != '\n'); // limpa buffer
    }

    /* Menu interativo */
    int opcao;
    Componente *arrTrabalho = NULL; /* cópia usada pelos algoritmos */
    long comparacoes = 0;
    double tempo = 0.0;
    char chave[MAX_NOME];

    do {
        printf("\n=== Menu ===\n");
        printf("1) Exibir componentes (original)\n");
        printf("2) Ordenar por Nome (Bubble Sort) e medir (recomendado para busca binária)\n");
        printf("3) Ordenar por Tipo (Insertion Sort) e medir\n");
        printf("4) Ordenar por Prioridade (Selection Sort) e medir\n");
        printf("5) Busca binária por Nome (após ordenar por nome) \n");
        printf("6) Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1;
        }
        while (getchar() != '\n'); // limpa '\n'

        switch (opcao) {
            case 1:
                mostrarComponentes(componentes, n);
                break;

            case 2: {
                /* copia para não alterar o array original (para poder testar outros algoritmos) */
                free(arrTrabalho);
                arrTrabalho = copiarArray(componentes, n);
                if (!arrTrabalho) {
                    printf("Erro ao alocar memória para cópia.\n");
                    break;
                }
                tempo = medirTempo(bubbleSortNome, arrTrabalho, n, &comparacoes);
                printf("\nResultado: Bubble Sort por Nome\n");
                printf("Comparacoes (strcmp): %ld\n", comparacoes);
                printf("Tempo CPU (s): %.6f\n", tempo);
                mostrarComponentes(arrTrabalho, n);
                /* informar que arrTrabalho está ordenado por nome (útil para busca) */
                break;
            }

            case 3: {
                free(arrTrabalho);
                arrTrabalho = copiarArray(componentes, n);
                if (!arrTrabalho) {
                    printf("Erro ao alocar memória para cópia.\n");
                    break;
                }
                tempo = medirTempo(insertionSortTipo, arrTrabalho, n, &comparacoes);
                printf("\nResultado: Insertion Sort por Tipo\n");
                printf("Comparacoes (strcmp): %ld\n", comparacoes);
                printf("Tempo CPU (s): %.6f\n", tempo);
                mostrarComponentes(arrTrabalho, n);
                break;
            }

            case 4: {
                free(arrTrabalho);
                arrTrabalho = copiarArray(componentes, n);
                if (!arrTrabalho) {
                    printf("Erro ao alocar memória para cópia.\n");
                    break;
                }
                tempo = medirTempo(selectionSortPrioridade, arrTrabalho, n, &comparacoes);
                printf("\nResultado: Selection Sort por Prioridade\n");
                printf("Comparacoes (int): %ld\n", comparacoes);
                printf("Tempo CPU (s): %.6f\n", tempo);
                mostrarComponentes(arrTrabalho, n);
                break;
            }

            case 5: {
                if (!arrTrabalho) {
                    printf("Aviso: nenhum vetor ordenado disponível. Ordene primeiro por Nome (opção 2).\n");
                    break;
                }
                /* Verifica se arrTrabalho está ordenado por nome de fato — assumimos que o usuário realizou op 2 */
                printf("Digite o nome do componente a buscar (exato): ");
                if (!fgets(chave, sizeof(chave), stdin)) chave[0] = '\0';
                trim_newline(chave);

                long compBusca = 0;
                int idx = buscaBinariaPorNome(arrTrabalho, n, chave, &compBusca);
                printf("Comparacoes realizadas na busca: %ld\n", compBusca);
                if (idx >= 0) {
                    printf("Componente encontrado no índice %d:\n", idx);
                    printf("Nome: %s | Tipo: %s | Prioridade: %d\n",
                           arrTrabalho[idx].nome, arrTrabalho[idx].tipo, arrTrabalho[idx].prioridade);
                } else {
                    printf("Componente \"%s\" nao encontrado.\n", chave);
                }
                break;
            }

            case 6:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }

    } while (opcao != 6);

    /* libera memória */
    free(componentes);
    free(arrTrabalho);

    return 0;
}
