//Trabalho incompleto -> não implementei a parte da remoção dos primos e o código apresenta comportamento indefinido


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

struct s_no {
    int num;
    struct s_no *prox;
    pthread_mutex_t lock;
};

typedef struct s_no s_no;

s_no *ptlista1 = NULL;

void *removerPares(void *param);
//void *removerPrimos(void *param);
void *imprimirPrimos(void *param);


void inicializaPt(s_no **ptlista1)
{
    //assegura que ptlista é numido e *ptlista nao foi iniciado ainda
    if ((ptlista1 != NULL) && (*ptlista1 == NULL)) {
        *ptlista1 = malloc(sizeof(s_no));

        if (*ptlista1 != NULL) {
            (*ptlista1)->prox = NULL;
            (*ptlista1)->num = -2;
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

void inserirFim(struct s_no* ptlista1, int valor) {

    //ptr = percorrer a lita, ant = anterior, lastAnt = antepenultimo
    s_no *ptr1 = ptlista1;
    s_no *ant = NULL;
    s_no *lastAnt = NULL;
     
    s_no *novo;

    //percorrer até o final, ant guarda o anterior de ptr, e lastAnt guarda anterior de ant
    while (ptr1->prox != NULL) {
        if (ant != NULL) {
            lastAnt = ant;
        }

        ant = ptr1;
        ptr1 = ptr1->prox;

        if (ptr1->prox == NULL) {
        
            if (lastAnt != NULL) {
                pthread_mutex_lock(&lastAnt->lock);
            }
                
            pthread_mutex_lock(&ant->lock);
            pthread_mutex_lock(&ptr1->lock);
        }
        
    }

    novo = malloc(sizeof(struct s_no));

    if (novo != NULL) {
        pthread_mutex_init(&novo->lock, NULL);
        novo->num = valor;
        novo->prox = NULL;

        if (ant == NULL) {
            ptr1->prox = novo;
        }
        else {
            ptr1->prox = novo;

            if (lastAnt != NULL) {
                pthread_mutex_unlock(&lastAnt->lock);
            }

            pthread_mutex_unlock(&ant->lock);
        }

        pthread_mutex_unlock(&ptr1->lock);
    }
}



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

    //ptr = percorrer a lita, ant = anterior, lastAnt = antepenultimo
    s_no *ptr1 = ptlista1;
    s_no *ant = NULL;
    s_no *lastAnt = NULL;
    
    //auxiliar para parada
    int auxPar = 1;

    //nó para remover
    s_no *temp;

    do {
        if ((ptr1->num > 2) && (ptr1->num % 2 == 0)) {
        
           
            temp = malloc(sizeof(struct s_no));

            if (lastAnt != NULL) {
                //trava  o antepenultimo nó
                pthread_mutex_lock(&lastAnt->lock);

                //trava o nó anterior 
                pthread_mutex_lock(&ant->lock);

                //arruma os ponteiros
                temp = ptr1;
                ptr1 = temp->prox;
                ant->prox = temp->prox;
                free(temp);
                pthread_mutex_unlock(&ant->lock); 

                pthread_mutex_unlock(&lastAnt->lock);
            }
            else { 
            
                //trava o nó anterior 
                pthread_mutex_lock(&ant->lock);


                //arruma os ponteiros
                temp = ptr1;
                ptr1 = temp->prox;
                ant->prox = temp->prox; 
                free(temp);

                pthread_mutex_unlock(&ant->lock); 
            }
        }
        else {
        

            if (ptr1->prox != NULL) {
                if (ant != NULL)
                    lastAnt = ant;

                ant = ptr1;
                ptr1 = ptr1->prox;
            }
        }

        if (ptr1 != NULL && ptr1->num == -1) {
            auxPar = 0;
        }


    } while (auxPar);

    pthread_exit(0);
}

/* 
void *removerPrimos(void *param) {

    //ptr = percorrer a lita, ant = anterior, lastAnt = antepenultimo
    s_no *ptr1 = ptlista1;
    s_no *ant = NULL;
    s_no *lastAnt = NULL;
    
    //auxiliar para parada
    int auxPrimo = 1;

    //nó para remover
    s_no *temp;

    do {
        if (!(isPrimo(ptr1->num))){
            temp = malloc(sizeof(struct s_no));

            if (lastAnt != NULL) {
                //trava  o antepenultimo nó
                pthread_mutex_lock(&lastAnt->lock);

                //trava o nó anterior 
                pthread_mutex_lock(&ant->lock);

                //arruma os ponteiros
                temp = ptr1;
                ptr1 = temp->prox;
                ant->prox = temp->prox;
                free(temp);
                pthread_mutex_unlock(&ant->lock); 

                pthread_mutex_unlock(&lastAnt->lock);
            }
            else { 
            
                //trava o nó anterior 
                pthread_mutex_lock(&ant->lock);


                //arruma os ponteiros
                temp = ptr1;
                ptr1 = temp->prox;
                ant->prox = temp->prox; 
                free(temp);

                pthread_mutex_unlock(&ant->lock); 
            }
        }
        else {
        

            if (ptr1->prox != NULL) {
                if (ant != NULL)
                    lastAnt = ant;

                ant = ptr1;
                ptr1 = ptr1->prox;
            }
        }

        if (ptr1 != NULL && ptr1->num == -1) {
            auxPrimo = 0;
        }


    } while (auxPrimo);

    pthread_exit(0);
}
*/

void *imprimirPrimos(void *param) {
    s_no *ptr1 = ptlista1;   
    s_no *next = NULL;      


    printf("Lista1: ");
    pthread_mutex_lock(&ptr1->lock);

    while (ptr1->prox != NULL && ptr1->prox->num != -1) {
        next = ptr1->prox;


        pthread_mutex_lock(&next->lock);


        printf("%d ", next->num);


        pthread_mutex_unlock(&ptr1->lock);

        ptr1 = next;
    }

    if (ptr1 != NULL) {
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
    //pthread_create(&tid[2], &attr3, removerPrimos, NULL);


    while (fscanf(arq, "%d", &valor) != EOF) {
        inserirFim(ptlista1, valor);
    }

    inserirFim(ptlista1, -1);

    fclose(arq);


    for (int i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }
        
    
    
    desalocarLista(ptlista1);

    return 0;
}
