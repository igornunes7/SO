#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

struct s_no {
    int num;
    struct s_no *prox;
    pthread_mutex_t lock; 
};
typedef struct s_no s_no;


s_no *ptlista1 = NULL;


void *removerPares(void *param);
void *removerPrimos(void *param);
void *imprimirPrimos(void *param);

//função para inicializar o nó cabeça da lista
void inicializaPt(s_no **ptlista) {
    if ((ptlista != NULL) && (*ptlista == NULL)) {
        *ptlista = malloc(sizeof(s_no));
        if (*ptlista != NULL) {
            (*ptlista)->prox = NULL;
            (*ptlista)->num = -1;
            pthread_mutex_init(&(*ptlista)->lock, NULL);
        } else {
            fprintf(stderr, "Erro na alocacao do no cabeca.\n");
            exit(-1);
        }
    } else {
        fprintf(stderr, "Erro no ponteiro ptlista\n");
        exit(-1);
    }
}

//função para inserir no final da lista1
void inserirFim(s_no **ptlista, int x) {
    s_no *novo = malloc(sizeof(s_no));
    novo->num = x;
    novo->prox = NULL;
    pthread_mutex_init(&novo->lock, NULL);

    s_no *ptr = *ptlista;
    while (ptr->prox != NULL) {
        ptr = ptr->prox;
    }

    pthread_mutex_lock(&novo->lock);
    ptr->prox = novo;
    pthread_mutex_unlock(&novo->lock);
}

//função auxiliar para verificar se o número é primo
bool isPrimo(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

//função para desalocar a lista
void desalocarLista(s_no *ptlista) {
    s_no *ptr1 = ptlista;
    s_no *aux;
    while (ptr1 != NULL) {
        aux = ptr1->prox;
        pthread_mutex_destroy(&ptr1->lock);
        free(ptr1);
        ptr1 = aux;
    }
}

//função para remover os pares maiores que 2 da lista1
void *removerPares(void *param) {
    s_no *ptr1;
    s_no *aux = NULL;
    ptr1 = ptlista1;

    while (ptr1 != NULL) {
        if (ptr1->num % 2 == 0 && ptr1->num > 2) {
            if (aux == NULL) {
                ptlista1 = ptr1->prox;
            } else {
                aux->prox = ptr1->prox;
            }
            s_no *temp = ptr1;
            pthread_mutex_lock(&ptr1->lock);
            ptr1 = ptr1->prox;
            pthread_mutex_unlock(&ptr1->lock);
            pthread_mutex_destroy(&temp->lock);
            free(temp);
        } else {
            aux = ptr1;
            ptr1 = ptr1->prox;
        }
    }
    pthread_exit(0);
}

//função para remover não primos da lista1
void *removerPrimos(void *param) {
    s_no *ptr1;
    s_no *aux = NULL;

    ptr1 = ptlista1;

    while (ptr1 != NULL) {
        if (!isPrimo(ptr1->num) && ptr1->num != -1) {
            if (aux == NULL) {
                ptlista1 = ptr1->prox;
            } else {
                aux->prox = ptr1->prox;
            }
            s_no *temp = ptr1;
            pthread_mutex_lock(&ptr1->lock);
            ptr1 = ptr1->prox;
            pthread_mutex_unlock(&ptr1->lock);
            pthread_mutex_destroy(&temp->lock);
            free(temp);
        } else {
            aux = ptr1;
            ptr1 = ptr1->prox;
        }
    }
    pthread_exit(0);
}

//função para imprimir a lista1
void *imprimirPrimos(void *param) {
    s_no *ptr1;
    
    ptr1 = ptlista1->prox;

    printf("Lista1: ");
    while (ptr1 != NULL) {
        printf("%d ", ptr1->num);
        pthread_mutex_lock(&ptr1->lock);
        ptr1 = ptr1->prox;
        pthread_mutex_unlock(&ptr1->lock);
    }
    printf("\n");
    pthread_exit(0);
}

int main(int argc, char **argv) {
    int valor = 0;
    
    FILE *arq;
    inicializaPt(&ptlista1);
    pthread_attr_t attr1, attr2, attr3;
    pthread_t tid[3];

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <file name>\n", argv[0]);
        exit(-1);
    }

    arq = fopen(argv[1], "rt");
    if (arq == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo\n");
        exit(-1);
    }

    pthread_create(&tid[0], &attr1, removerPares, NULL);
    pthread_create(&tid[1], &attr2, removerPrimos, NULL);
    pthread_create(&tid[2], &attr3, imprimirPrimos, NULL);

    while (fscanf(arq, "%d", &valor) != EOF) {
        inserirFim(&ptlista1, valor);
    }

    fclose(arq);

    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    desalocarLista(ptlista1);
    pthread_mutex_destroy(&ptlista1->lock);

    return 0;
}
