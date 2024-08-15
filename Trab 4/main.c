#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>   
#include <sys/stat.h> 
#include <stdbool.h>

#define sem_1 "lista1"
#define sem_2 "lista2"
#define sem_3 "lista3"
#define sem_fim "fimSem"

struct s_no {
    int num;
    struct s_no *prox;
};

typedef struct s_no s_no;


s_no *ptlista1 = NULL;
s_no *ptlista2 = NULL;
s_no *ptlista3 = NULL;


//semaforo para lista1
sem_t *sem1;
//semaforo para lista2
sem_t *sem2;
//semaforo para lista3
sem_t *sem3;
//semaforo para fim
sem_t *fim;


void *removerPares(void *param);
void *removerPrimos(void *param);
void *imprimirPrimos(void *param);


//função para inicializar os semaforos
void inicializaSem () {
    sem1 = sem_open(sem_1, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem2 = sem_open(sem_2, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem3 = sem_open(sem_3, O_CREAT, S_IRUSR | S_IWUSR, 0);
    fim = sem_open(sem_fim, O_CREAT, S_IRUSR | S_IWUSR, 0);

    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED || sem3 == SEM_FAILED || fim == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
}

//função usada para inicilziar no cabeça das listas
void inicializaPt(s_no **ptlista)
{
    //assegura que ptlista é valido e *ptlista nao foi iniciado ainda
    if ((ptlista != NULL) && (*ptlista == NULL)) {
        *ptlista = malloc(sizeof(s_no));

        if (*ptlista != NULL) {
            (*ptlista)->prox = NULL;
            (*ptlista)->num = -1;
        }
        else {
            fprintf(stderr, "Erro na alocacao do no cabeca.\n");
            exit(-1);
        }
    }
    else {
        fprintf(stderr, "Erro no ponteiro ptlista\n");
        exit(-1);
    }
}


//inserir no final da lista1
void inserirFim(s_no **ptlista, int x) {
    s_no *novo = malloc(sizeof(s_no));
    novo->num = x;
    novo->prox = NULL;

    s_no *ptr = *ptlista;

    while (ptr->prox != NULL) {
        ptr = ptr->prox;
    }

    ptr->prox = novo;

    //sinaliza que lista1 recebeu um novo elemento
    sem_post(sem1);

}

//função auxiliar para verificar se o número é primo
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


//função para remover os pares maiores que 2 da lista1
void *removerPares(void *param) {
    s_no *ptr1 = ptlista1;
    s_no *ptr2 = ptlista2;
    s_no *novo;

    //esperando acesso da lista1
    sem_wait(sem1);

    do {
        if (!(ptr1->prox->num % 2 == 0 && ptr1->prox->num > 2)) {
            novo = malloc(sizeof(s_no));

            if (novo != NULL) {
                novo->num = ptr1->prox->num;
                novo->prox = NULL;

                ptr2->prox = novo;
                ptr2 = ptr2->prox;

                //sinaliza que lista2 recebeu um novo elemento
                sem_post(sem2);
            }
        }
        //garante que lista1 nao seja mudada por outra thread enquanto está em andamento
        sem_wait(sem1);
        ptr1 = ptr1->prox;

    } while (ptr1->prox->num != -1);


    //indicar que a lista2 acabou
    sem_post(sem2);

    //espera sinalização do semaforo fim antes de terminar sua execução
    sem_wait(fim);

    //indica que a thread terminou
    sem_post(fim);

    //fecha o semaforo
    sem_close(sem2);


    //printf("Thread removerPares terminou\n");
    pthread_exit(0);
}


//função para remover não primos da lista2
void *removerPrimos(void *param) {
    s_no *ptr2 = ptlista2;
    s_no *ptr3 = ptlista3;
    s_no *novo;

    //esperando acesso da lista2
    sem_wait(sem2);
    do {
        if (isPrimo(ptr2->prox->num)) {
            novo = malloc(sizeof(s_no));

            if (novo != NULL) {
                novo->num = ptr2->prox->num;
                novo->prox = NULL;
                ptr3->prox = novo;

                ptr3 = ptr3->prox;

                //sinaliza que lista3 recebeu um novo elemento
                sem_post(sem3);
            }
        }
        //garante que lista2 nao seja mudada por outra thread enquanto está em andamento
        sem_wait(sem2);
        ptr2 = ptr2->prox;

    } while (ptr2->prox != NULL);

    //indicar que a lista3 acabou
    sem_post(sem3);

    //espera sinalização do semaforo fim antes de terminar sua execução
    sem_wait(fim);


    //indica que a thread terminou
    sem_post(fim);

    //fecha o semaforo
    sem_close(sem3);


    //printf("Thread removerPrimos terminou\n");
    pthread_exit(0);
}


//função para imprimir a lista3
void *imprimirPrimos(void *param) {
    s_no *ptr3 = ptlista3;
    
    //esperando acesso da lista3
    sem_wait(sem3);
    
    printf("Lista3: ");
    do {
        printf("%d ", ptr3->prox->num);

        //garante que nao tente imprimir um nó que nao foi alocado ainda
        sem_wait(sem3);

        ptr3 = ptr3->prox;
    } while (ptr3->prox != NULL);

    printf("\n");

    //sinaliza que a thread terminou para permirtir que as outras terminem tbm
    sem_post(fim);


    //printf("Thread imprimirPrimos terminou\n");
    pthread_exit(0);
}


//desalocar a lista
void desalocarLista(s_no **ptlista) {
    s_no *ptr = *ptlista;
    s_no *aux;

    while (ptr != NULL) {
        aux = ptr->prox;
        free(ptr);
        ptr = aux;
    }

    *ptlista = NULL; 
}

int main(int argc, char **argv) {
    sem_unlink(sem_1);
    sem_unlink(sem_2);
    sem_unlink(sem_3);
    sem_unlink(sem_fim);

    pthread_attr_t attr;
    pthread_t tid[3];
    FILE *arq;
    int valor;

    inicializaPt(&ptlista1);
    inicializaPt(&ptlista2);
    inicializaPt(&ptlista3);

    inicializaSem();

    if (argc != 2) {
        fprintf(stderr, "usage: %s <file name>\n", argv[0]);
        exit(-1);
    }

    arq = fopen(argv[1], "rt");
    if (arq == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo\n");
        exit(-1);
    }

    pthread_attr_init(&attr);
    pthread_create(&tid[0], &attr, removerPares, NULL);
    pthread_create(&tid[1], &attr, removerPrimos, NULL);
    pthread_create(&tid[2], &attr, imprimirPrimos, NULL);

    while (fscanf(arq, "%d", &valor) != EOF) {
        inserirFim(&ptlista1, valor);
    }

    inserirFim(&ptlista1, -1);
    fclose(arq);    


    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }
    

    sem_close(sem1);
    sem_close(fim);
    sem_unlink(sem_1);
    sem_unlink(sem_2);
    sem_unlink(sem_3);
    sem_unlink(sem_fim);

    desalocarLista(&ptlista1);
    desalocarLista(&ptlista2);
    desalocarLista(&ptlista3);
    
    return 0;
}


