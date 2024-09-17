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
s_no *ptlista1_fim = NULL;
pthread_mutex_t lock;

void *removerPares(void *param);
void *removerPrimos(void *param);
void *imprimirPrimos(void *param);


bool isPrimo(int n) {
    if (n <= 1) {
        return false;
    }
    if (n == 2) {
        return true;
    }
    if (n % 2 == 0) {
        return false;
    }
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

void desalocarLista(s_no *ptlista) {
    s_no *ptr1 = ptlista;
    s_no *aux;



    while (ptr1 != NULL) {
        aux = ptr1->prox;
        pthread_mutex_destroy(&ptr1->lock);
        free(ptr1);
        ptr1 = aux;
    }
    ptlista = NULL;
}


void *removerPares(void *param) {
    while (ptlista1->prox == NULL) {

    }

    s_no *ptr1 = ptlista1;
    s_no *aux = NULL;

    while (ptr1->num != -1) {
        pthread_mutex_lock(&ptr1->lock);
        if (ptr1->num % 2 == 0 && ptr1->num > 2) {
            s_no *temp = ptr1;
            if (aux == ptlista1) {
                aux->prox = NULL;
                ptr1 = ptlista1;
            } else {
                aux->prox = NULL;
                ptr1 = aux;
            }
            pthread_mutex_unlock(&ptr1->lock);
            ptr1 = ptr1->prox;
            pthread_mutex_destroy(&temp->lock);
            free(temp);
        } else {
            pthread_mutex_unlock(&ptr1->lock);
            aux = aux->prox;
            ptr1 = ptr1->prox;
        }
    }

    pthread_exit(0);
}


/*
void *removerPrimos(void *param) {
    s_no *ptr1 = ptlista1;
    s_no *aux = NULL;

    while (ptr1 != NULL) {
        if (!isPrimo(ptr1->num) && ptr1->num != -1) {
            pthread_mutex_lock(&ptr1->lock);
            if (aux == NULL) {
                ptlista1->prox = ptr1->prox;
            } else {
                aux->prox = ptr1->prox;
            }
            s_no *temp = ptr1;
            ptr1 = ptr1->prox;

            pthread_mutex_unlock(&temp->lock);
            pthread_mutex_destroy(&temp->lock);
            free(temp);
        } else {
            aux = ptr1;
            ptr1 = ptr1->prox;
        }
    }

    pthread_exit(0);
}

*/

void *imprimirPrimos(void *param) {
    while (ptlista1->prox == NULL) {
        
    }
    s_no *ptr1 = ptlista1->prox;

    printf("Lista1: ");
    while (ptr1->num != -1) {
        printf("%d ", ptr1->num);
        ptr1 = ptr1->prox;
    }

    printf("\n");
    pthread_exit(0);
}

int main(int argc, char **argv) {
    int valor = 0;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    FILE *arq;

    pthread_attr_t attr1, attr2;
    pthread_t tid[2];
    s_no *ptr = NULL;

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);

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
    pthread_create(&tid[1], &attr2, imprimirPrimos, NULL);


    while (fscanf(arq, "%d", &valor) != EOF) {
        s_no *novo = (s_no *)malloc(sizeof(s_no));
        s_no *ptr = ptlista1;
        novo->num = valor;
        novo->prox = NULL;
        pthread_mutex_init(&novo->lock, NULL);

        if (ptlista1 == NULL) {
            ptlista1 = novo;
            ptlista1_fim = novo;
            pthread_mutex_lock(&novo->lock);
        } else {
            ptlista1_fim->prox = novo;
            ptlista1_fim = novo;
            pthread_mutex_lock(&novo->lock);
            pthread_mutex_unlock(&ptr->lock);
            ptr = ptr->prox;
        }

    }

    s_no *ultimo = (s_no *)malloc(sizeof(s_no));
    s_no *ptr = ptlista1;
    ultimo->num = -1;
    ultimo->prox = NULL;
    ptlista1_fim->prox = ultimo;
    ptlista1_fim = ultimo;
    fclose(arq);

    for (int i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }

    desalocarLista(ptlista1);

    return 0;
}
