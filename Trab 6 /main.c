#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


int countAccesses(FILE *arq) {
    int count = 0;
    int temp;
    

    while (fscanf(arq, "%d", &temp) == 1) {
        count++;
    }

    //retorna o ponteiro do arquivo para o início
    rewind(arq);
    return count;
}

//auxiliar para troca
void swap (int memory[], int ant, int prox){
    int temp = memory[ant];

    memory[ant] = memory[prox];

    memory[prox] = temp;
}


//(LRU) move o elemento na posição index para o final da lista
void moveToLast(int index, int memory[], int size)
{
    for (int i = index; i <= size - 2; i++) {
        swap(memory, i, i + 1);
    }
}


//verifica se a página já está carregada na memória
int verify(int num, int memory[], int size)
{
    for (int i = 0; i < size; i++) {
        if (memory[i] == num) {
            return i;
        }
    }

    return -1;
}


int FIFO(int acessos[], int tam_pagina, int qtd_pag, int num_acessos, FILE *fptr, int tam) {
    int memoria_fifo[tam];  
    int primero = 0;       
    int n = 0;              
    int erro_fifo = 0;     


    fprintf(fptr, "FIFO:\n");


    for (int j = 0; j < num_acessos; j++) {
        //a pagina nao está na memoria (erro de pagina)
        if (verify(acessos[j] / tam_pagina, memoria_fifo, n) == -1) {

            //se há espaço na memoria
            if (n < qtd_pag) { 
                memoria_fifo[n] = acessos[j] / tam_pagina; 
                erro_fifo++;
                n++;
                fprintf(fptr, "Erro de página. Endereço: %d, Página: %d\n", acessos[j], acessos[j] / tam_pagina);

            //memoria cheia (substituição de pag)
            } else { 
                memoria_fifo[primero] = acessos[j] / tam_pagina; 
                primero++;

                //se primeiro atingir o fim da memória, volta ao início
                if (primero == n) {
                    primero = 0; 
                }
                erro_fifo++;
                fprintf(fptr, "Erro de página. Endereço: %d, Página: %d\n", acessos[j], acessos[j] / tam_pagina);
            }
        } else {
           
        }
    }

    return erro_fifo;
}


int OPT(int acessos[], int tam_pagina, int qtd_pag, int num_acessos, FILE *fptr, int tam) {
    int memoria_opt[tam];     
    int n = 0;              
    int erro_opt = 0;     
    int menor;
    int menorcount = 0;
    int count = 0;

    fprintf(fptr, "\nOPT:\n");


    for (int j = 0; j < num_acessos; j++) {
        //a pagina nao está na memoria (erro de pagina)
        if (verify(acessos[j] / tam_pagina, memoria_opt, n) == -1) {

            //se há espaço na memoria
            if (n < qtd_pag) { 
                memoria_opt[n] = acessos[j] / tam_pagina; 
                erro_opt++;
                n++;
                fprintf(fptr, "Erro de página. Endereço: %d, Página: %d\n", acessos[j], acessos[j] / tam_pagina);

            //memoria cheia (substituição de pag)
            } else { 
                menor = 0;
                //for auxiliar pra percorrer o arquivo todo
                for (int k = 0; k < n; k++) {
                    count = 0;

                    //for que vai do endereço onde está ate o final
                    for (int i = j; i < num_acessos; i++) {
                        if (memoria_opt[k] == acessos[i]/tam_pagina) {
                            count++;
                        }
                    }
                    //assumindo que a pag a ser substituida é a atual
                    if (k == 0) {
                        menorcount = count;
                    } else {
                        if (count < menorcount) {
                            //atualizar indice da pagina menos usada
                            menor = k; 
                        }
                    }
                }
                erro_opt++;
                memoria_opt[menor] = acessos[j]/tam_pagina;
                fprintf(fptr, "Erro de página. Endereço: %d, Página: %d\n", acessos[j], acessos[j] / tam_pagina);
            }
        } else {
           
        }
    }

    return erro_opt;
}


int LRU(int acessos[], int tam_pagina, int qtd_pag, int num_acessos, FILE *fptr, int tam) {
    int memoria_lru[tam];        
    int n = 0;              
    int erro_lru = 0; 
    int usado;    


    fprintf(fptr, "\nLRU:\n");


    for (int j = 0; j < num_acessos; j++) {
        //a pagina nao está na memoria (erro de pagina)
        if((usado = verify(acessos[j]/tam_pagina, memoria_lru, n)) == -1) {

            //se há espaço na memoria
            if (n < qtd_pag) { 
                memoria_lru[n] = acessos[j] / tam_pagina; 
                erro_lru++;
                n++;
                fprintf(fptr, "Erro de página. Endereço: %d, Página: %d\n", acessos[j], acessos[j] / tam_pagina);

            //memoria cheia (substituição de pag)
            } else { 
                memoria_lru[0] = acessos[j] / tam_pagina; 
                erro_lru++;
                fprintf(fptr,"erro de pagina endereço :%d pagina: %d\n", acessos[j], acessos[j] / tam_pagina);
            }
        } else {
            //atualiza pagina menos usada
            moveToLast(usado, memoria_lru, n);
        }
    }

    return erro_lru;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: %s <tam_pagina> <tam_memoria> <arquivo_acesso>\n", argv[0]);
        exit(1);
    }

    int tam_pagina = atoi(argv[1]);
    int tam_memoria = atoi(argv[2]);


    FILE *qtd_adress = fopen(argv[3], "r");
    if (qtd_adress == NULL) {
        printf("Erro ao abrir o arquivo de acessos.\n");
        exit(1);
    }

    int num_acessos = countAccesses(qtd_adress);
    int32_t *acessos = (int32_t*) malloc(num_acessos * sizeof(int32_t));
    
    if (acessos == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    int tam = 0;
    while (fscanf(qtd_adress, "%d", &acessos[tam]) == 1) {
        tam++;
    }

    fclose(qtd_adress);


    int qtd_pag = tam_memoria / tam_pagina;


    FILE *arq_exit = fopen("erros.out", "w");
    if (arq_exit == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        exit(1);
    }


    int erro_fifo = FIFO(acessos, tam_pagina, qtd_pag, num_acessos, arq_exit, tam);

    printf("\nErros FIFO: %d\n", erro_fifo);
    float percentual_fifo = (float) erro_fifo / num_acessos * 100;
    printf("Percentual de erros: %.2f\n", percentual_fifo);


    int erro_opt = OPT(acessos, tam_pagina, qtd_pag, num_acessos, arq_exit, tam);

    printf("\nErros OPT: %d\n", erro_opt);
    float percentual_opt = (float) erro_opt / num_acessos * 100;
    printf("Percentual de erros: %.2f\n", percentual_opt);


    int erro_lru = LRU(acessos, tam_pagina, qtd_pag, num_acessos, arq_exit, tam);

    printf("\nErros LRU: %d\n", erro_lru);
    float percentual_lru = (float) erro_lru / num_acessos * 100;
    printf("Percentual de erros: %.2f\n", percentual_lru);


    fclose(arq_exit);
    return 0;
}