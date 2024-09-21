//Trabalho Incompleto -> Faltou o -seq porém os que estão implementados não funcionam em alguns casos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct s_proc {
    int id;
    int prioridade;
    int chegada;
    int *picoCPU;
    int *picoES;
    int tamCPU;
    int tamES;
    int tempoInicio;
    int tempoTermino;
    int tempoDisponivel;
    int auxCPU;
    int auxES;
    int contCPU;
    int contES;
    int jaFoi;
    int acabou;
    int interrompido;
    int auxPicoCPU;
};

struct s_no {
    struct s_proc processo;
    struct s_no *prox;
};

//insere no final da lista encadeada
void insereLista(struct s_no** ptLista, char *parametros, int idProc) {
    struct s_no *ptr = *ptLista;
    struct s_no *novoProcesso = malloc(sizeof(struct s_no));

    novoProcesso->processo.id = idProc;
    novoProcesso->processo.picoCPU = malloc(sizeof(int));
    novoProcesso->processo.picoES = malloc(sizeof(int));
    novoProcesso->processo.picoES[0] = 0;
    novoProcesso->processo.picoCPU[0] = 0;
    novoProcesso->prox = NULL;
    novoProcesso->processo.tamCPU = 1;
    novoProcesso->processo.tamES = 1;
    novoProcesso->processo.tempoInicio = 0;
    novoProcesso->processo.tempoTermino = 0;
    novoProcesso->processo.tempoDisponivel = 0;
    novoProcesso->processo.auxCPU = 0;
    novoProcesso->processo.auxES = 0;
    novoProcesso->processo.contCPU = 1;
    novoProcesso->processo.contES = 1;
    novoProcesso->processo.jaFoi = 0;
    novoProcesso->processo.acabou = 0;
    novoProcesso->processo.interrompido = 0;

    int indice = 1;
    char *ptToken;

    ptToken = strtok(parametros, " ");
    while (ptToken) {
        if (indice == 1) {
            novoProcesso->processo.prioridade = atoi(ptToken);
        } else if (indice == 2) {
            novoProcesso->processo.chegada = atoi(ptToken);
        } else {
            if (indice % 2 != 0) {
                novoProcesso->processo.picoCPU = realloc(novoProcesso->processo.picoCPU, novoProcesso->processo.tamCPU * sizeof(int));
                novoProcesso->processo.picoCPU[novoProcesso->processo.tamCPU - 1] = atoi(ptToken);
                novoProcesso->processo.tamCPU++;

            } else {
                novoProcesso->processo.picoES = realloc(novoProcesso->processo.picoES, novoProcesso->processo.tamES * sizeof(int));
                novoProcesso->processo.picoES[novoProcesso->processo.tamES - 1] = atoi(ptToken);
                novoProcesso->processo.tamES++;
            }
        }

        ++indice;
        ptToken = strtok(NULL, " ");
    }

    if (*ptLista == NULL) {
        *ptLista = novoProcesso;
    } else {
        while (ptr->prox != NULL)
            ptr = ptr->prox;
        ptr->prox = novoProcesso;
    }
}


void leArquivo(struct s_no** ptLista, const char* nomeArquivo) {
    FILE *arq = fopen(nomeArquivo, "rt");
    if (arq == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo\n");
        exit(-1);
    } else {
        char linhaArquivo[100];
        int idProc = 1;

        while ((fgets(linhaArquivo, 100, arq)) != NULL) {
            insereLista(ptLista, linhaArquivo, idProc);
            idProc++;
        }

        fclose(arq);
    }
}


void limpaLista(struct s_no** ptLista) {
    struct s_no *ptr = *ptLista;
    while (ptr != NULL) {
        struct s_no *temp = ptr;
        ptr = ptr->prox;
        free(temp->processo.picoCPU);
        free(temp->processo.picoES);
        free(temp);
    }
    *ptLista = NULL;
}

void FCFS(FILE *file, struct s_no* ptLista) {
    struct s_no *ptr = ptLista;
    int tempoAtual = 0;

    fprintf(file, "FCFS:\n");
    fprintf(file, "%d[", ptr->processo.chegada);
    while (ptr != NULL) {
        if (tempoAtual < ptr->processo.chegada) {
            tempoAtual = ptr->processo.chegada;
        }
        ptr->processo.tempoInicio = tempoAtual;
        tempoAtual += ptr->processo.picoCPU[0];
        ptr->processo.tempoTermino = tempoAtual;
        ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[0];

        fprintf(file, "P%d %d", ptr->processo.id, ptr->processo.tempoTermino);
        if (ptr->prox != NULL) {
            fprintf(file, "|");
        }

        ptr = ptr->prox;
    }
    fprintf(file, "|");


    int processosRestantes = 1;
    while (processosRestantes == 1) {
        processosRestantes = 0;
        ptr = ptLista;
        int nextProcTempoDisponivel = -1;
        int todosProcessadoresExecutados = 1;  
        int todosAcabaram = 1;  


        while (ptr != NULL) {
            if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                ptr->processo.acabou = 1;
            }

            if (ptr->processo.acabou == 1) {
                ptr->processo.jaFoi = 1;
            }


            if (ptr->processo.tempoDisponivel > tempoAtual) {
                if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                    nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                }
                processosRestantes = 1;
            }
          
            if (ptr->processo.jaFoi == 0) {
                if (ptr->processo.tempoDisponivel < tempoAtual) {
                    if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 1) {
                            nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                            processosRestantes = 1;
                        }

                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 0) {
                            if (ptr->processo.tempoDisponivel < ptr->prox->processo.tempoDisponivel) {
                                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                                processosRestantes = 1;
                            } else {
                                nextProcTempoDisponivel = ptr->prox->processo.tempoDisponivel;
                                processosRestantes = 1;
                            }
                            
                        }
                    }
                }
                todosProcessadoresExecutados = 0;
            }

            if (ptr->processo.acabou == 0) {
                todosAcabaram = 0;
            }
            
            ptr = ptr->prox;
        }

        if (todosAcabaram) {
            break;
        }

        if (todosProcessadoresExecutados != 0) {
            ptr = ptLista;
            while (ptr != NULL) {
                ptr->processo.jaFoi = 0;
                ptr = ptr->prox;
            }
        }

        if (nextProcTempoDisponivel > tempoAtual) {
            fprintf(file, "*** %d| ", nextProcTempoDisponivel);
            tempoAtual = nextProcTempoDisponivel;
        }

        ptr = ptLista;
        while (ptr != NULL) {
            if (ptr->processo.tempoDisponivel == tempoAtual && ptr->processo.jaFoi == 0) {
                fprintf(file, "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }
                ptr->processo.jaFoi = 1;
                break;
            }

            if (ptr->processo.tempoDisponivel == nextProcTempoDisponivel && ptr->processo.jaFoi == 0) {
                fprintf(file, "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                } 
                    
            
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                ptr->processo.jaFoi = 1;
                break;
            }
            ptr = ptr->prox;
        }
    }
    
    fprintf(file, "]\n");
    fprintf(file, "\n");
}

void SJF(FILE *file, struct s_no* ptLista) {
    struct s_no *ptr = ptLista;

    struct s_no *nextProc;
    int tempoAtual = 0;
    int processosRestantes = 1;
    int totalProcessos = 0;
    
    fprintf(file, "SJF:\n");
    fprintf(file, "%d[", ptr->processo.chegada);
    tempoAtual = ptr->processo.chegada + ptr->processo.picoCPU[0];
    fprintf(file, "P%d %d|", ptr->processo.id, tempoAtual);
    ptr->processo.jaFoi = 1;
    ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[0];
    ptr->processo.auxCPU++;


    while (processosRestantes > 0) {
        processosRestantes = 0;
        nextProc = NULL;
        ptr = ptLista;

        while (ptr != NULL) {
            if (ptr->processo.jaFoi == 0 && (nextProc == NULL || ptr->processo.picoCPU[0] < nextProc->processo.picoCPU[0])) {
                nextProc = ptr;
            }
            if (ptr->processo.jaFoi == 0) {
                processosRestantes++;
            }
            ptr = ptr->prox;
        }

        if (nextProc != NULL) {
            tempoAtual += nextProc->processo.picoCPU[0];
            nextProc->processo.jaFoi = 1;
            nextProc->processo.tempoDisponivel = tempoAtual + nextProc->processo.picoES[0];
            nextProc->processo.auxCPU++;
            

            fprintf(file, "P%d %d|", nextProc->processo.id, tempoAtual);
        }
    }

    ptr = ptLista;
    while (ptr != NULL) {
        ptr->processo.jaFoi = 0;
        if (ptr->processo.auxCPU == ptr->processo.tamCPU - 1) {
            ptr->processo.jaFoi = 1;
            ptr->processo.acabou = 1;
        }
    
        ptr->processo.auxCPU--;
        ptr = ptr->prox;
    }

    processosRestantes = 1;
    while (processosRestantes == 1) {
        processosRestantes = 0;
        ptr = ptLista;
        int nextProcTempoDisponivel = -1;
        int todosProcessadoresExecutados = 1;  
        int todosAcabaram = 1; 

        while (ptr != NULL) {
            if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                ptr->processo.acabou = 1;
            }

            if (ptr->processo.acabou == 1) {
                ptr->processo.jaFoi = 1;
            }


            if (ptr->processo.tempoDisponivel > tempoAtual) {
                if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                    nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                }
                processosRestantes = 1;
            }
          
            if (ptr->processo.jaFoi == 0) {
                if (ptr->processo.tempoDisponivel < tempoAtual) {
                    if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 1) {
                            nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                            processosRestantes = 1;
                        }

                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 0) {
                            if (ptr->processo.tempoDisponivel < ptr->prox->processo.tempoDisponivel) {
                                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                                processosRestantes = 1;
                            } else {
                                nextProcTempoDisponivel = ptr->prox->processo.tempoDisponivel;
                                processosRestantes = 1;
                            }
                            
                        }
                    }
                }
                todosProcessadoresExecutados = 0;
            }

            if (ptr->processo.acabou == 0) {
                todosAcabaram = 0;
            }
            
            ptr = ptr->prox;
        }

        if (todosAcabaram) {
            break;
        }

        if (todosProcessadoresExecutados != 0) {
            ptr = ptLista;
            while (ptr != NULL) {
                ptr->processo.jaFoi = 0;
                ptr = ptr->prox;
            }
        }

        if (nextProcTempoDisponivel > tempoAtual) {
            fprintf(file, "*** %d|", nextProcTempoDisponivel);
            tempoAtual = nextProcTempoDisponivel;
        }

        ptr = ptLista;
        while (ptr != NULL) {
            if (ptr->processo.tempoDisponivel == tempoAtual && ptr->processo.jaFoi == 0) {
                fprintf(file, "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }
                ptr->processo.jaFoi = 1;
                break;
            }

            if (ptr->processo.tempoDisponivel == nextProcTempoDisponivel && ptr->processo.jaFoi == 0) {
                fprintf(file, "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                } 
                    
            
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                ptr->processo.jaFoi = 1;
                break;
            }
            
            ptr = ptr->prox;
        }

        ptr = ptLista;
    }
    fprintf(file, "]\n");
    fprintf(file, "\n");
}


void SRTF(FILE *file, struct s_no* ptLista) {
    struct s_no *ptr = ptLista;
    struct s_no *nextProc;
    int tempoAtual = 0;
    int processosRestantes = 1;
    int totalProcessos = 0;
    int aux = 0;
    int aux2 = 0;
    int aux3 = 0;

    fprintf(file , "SRTF:\n");
    fprintf(file , "%d[", ptr->processo.chegada);

    aux = ptr->prox->processo.chegada - ptr->processo.chegada;
    tempoAtual = ptr->processo.chegada + aux;
    ptr->processo.picoCPU[0] -= aux;
    ptr->processo.interrompido = 1;

    fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual);

    tempoAtual += ptr->prox->processo.picoCPU[0];

    fprintf(file , "P%d %d|", ptr->prox->processo.id, tempoAtual);

    ptr->prox->processo.jaFoi = 1;
    ptr->prox->processo.tempoDisponivel = tempoAtual + ptr->prox->processo.picoES[0];


    while (processosRestantes > 0) {
        aux3 = 0;
        processosRestantes = 0;
        nextProc = NULL;
        ptr = ptLista;

        while (ptr != NULL) {
            if (ptr->processo.jaFoi == 0 && (nextProc == NULL || ptr->processo.picoCPU[0] < nextProc->processo.picoCPU[0])) {
                nextProc = ptr;
            }
            if (ptr->processo.jaFoi == 0) {
                processosRestantes++;
            }
    
            ptr = ptr->prox;
        }

        if (nextProc != NULL) {
            ptr = ptLista;
            while (ptr != NULL) {
                if (tempoAtual + nextProc->processo.picoCPU[0] > ptr->processo.tempoDisponivel && ptr->processo.tempoDisponivel != 0) {
                    aux2 = tempoAtual + nextProc->processo.picoCPU[0];
                    nextProc->processo.picoCPU[0] = aux2 - ptr->processo.tempoDisponivel;
                    nextProc->processo.tempoDisponivel = tempoAtual + nextProc->processo.picoES[0];
                    tempoAtual = ptr->processo.tempoDisponivel;

                    fprintf(file , "P%d %d|", nextProc->processo.id, tempoAtual);
                    aux3 = 1;
                    processosRestantes = -1;
                    nextProc->processo.interrompido = 1;
                    break;
                }
                ptr = ptr->prox;
            }

            if (aux3 != 1) {
                tempoAtual += nextProc->processo.picoCPU[0];
                nextProc->processo.jaFoi = 1;
                nextProc->processo.tempoDisponivel = tempoAtual + nextProc->processo.picoES[0];
                nextProc->processo.interrompido = 0;


                fprintf(file , "P%d %d|", nextProc->processo.id, tempoAtual);
            }
        }
    }
    

    ptr = ptLista;
    while (ptr != NULL) {
        ptr->processo.jaFoi = 0;
        ptr = ptr->prox;
    }


    ptr = ptLista;
    while (ptr != NULL) {
        if (ptr->processo.tempoDisponivel == tempoAtual) {
            tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
            fprintf (file , "P%d %d|", ptr->processo.id ,tempoAtual);
            ptr->processo.jaFoi = 1;
            ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];
            ptr->processo.auxCPU++;
            ptr->processo.auxES++;


            if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                ptr->processo.contCPU++;
            }
            if (ptr->processo.auxES < ptr->processo.tamES) {
                ptr->processo.contES++;
            }

            if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                ptr->processo.acabou = 1;
            }
            
        }
        ptr = ptr->prox;
    }

    processosRestantes = 1;
    while (processosRestantes == 1) {
        processosRestantes = 0;
        ptr = ptLista;
        int nextProcTempoDisponivel = -1;
        int todosProcessadoresExecutados = 1;  
        int todosAcabaram = 1;  


        while (ptr != NULL) {
            //cont e tam = 1
            //aux = 0
            if (ptr->processo.interrompido == 1) {
                tempoAtual += ptr->processo.picoCPU[0];
                fprintf (file , "P%d %d|", ptr->processo.id ,tempoAtual);
                ptr->processo.jaFoi = 1;
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[0];
                ptr->processo.interrompido = 0;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                    ptr->processo.acabou = 1;
                }
                break;
            }

            if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                ptr->processo.acabou = 1;
            }

            if (ptr->processo.acabou == 1) {
                ptr->processo.jaFoi = 1;
            }

            if (ptr->processo.tempoDisponivel > tempoAtual) {
                if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                    nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                }
                processosRestantes = 1;
            }
          
            if (ptr->processo.jaFoi == 0) {
                if (ptr->processo.tempoDisponivel < tempoAtual) {
                    if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 1) {
                            nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                            processosRestantes = 1;
                        }

                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 0) {
                            if (ptr->processo.tempoDisponivel < ptr->prox->processo.tempoDisponivel) {
                                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                                processosRestantes = 1;
                            } else {
                                nextProcTempoDisponivel = ptr->prox->processo.tempoDisponivel;
                                processosRestantes = 1;
                            }
                            
                        }
                    }
                }
                todosProcessadoresExecutados = 0;
            }
            
            if (ptr->processo.jaFoi == 0 && ptr->processo.tempoDisponivel == tempoAtual) {
                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                break;
            }
            if (ptr->processo.acabou == 0) {
                todosAcabaram = 0;
            }
            
            ptr = ptr->prox;
        }

        if (todosAcabaram) {
            break;
        }

        if (todosProcessadoresExecutados != 0) {
            ptr = ptLista;
            while (ptr != NULL) {
                ptr->processo.jaFoi = 0;
                ptr = ptr->prox;
            }
        }

        if (nextProcTempoDisponivel > tempoAtual) {
            fprintf(file , "*** %d|", nextProcTempoDisponivel);
            tempoAtual = nextProcTempoDisponivel;
        }

        ptr = ptLista;
        while (ptr != NULL) {
            if (ptr->processo.tempoDisponivel == tempoAtual && ptr->processo.jaFoi == 0) {
                fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }
                ptr->processo.jaFoi = 1;
                break;
            }

            if (ptr->processo.tempoDisponivel == nextProcTempoDisponivel && ptr->processo.jaFoi == 0) {
                fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                } 
                    
            
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                ptr->processo.jaFoi = 1;
                break;
            }
            ptr = ptr->prox;
        }
    }
    
    fprintf(file , "]\n");
    fprintf(file , "\n");


}

void PRIO_PRMP(FILE *file, struct s_no* ptLista) {
    struct s_no *ptr = ptLista;
    struct s_no *nextProc;
    int tempoAtual = 0;
    int processosRestantes = 1;
    int totalProcessos = 0;
    int aux = 0;
    int aux2 = 0;
    int aux3 = 0;

    fprintf(file , "PRIORIDADE PREEMPTIVO:\n");
    fprintf(file , "%d[", ptr->processo.chegada);

    aux = ptr->prox->processo.chegada - ptr->processo.chegada;
    tempoAtual = ptr->processo.chegada + aux;
    ptr->processo.tempoDisponivel = ptr->processo.chegada + ptr->processo.picoCPU[0];
    ptr->processo.picoCPU[0] -= aux;


    fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual);

    tempoAtual += ptr->prox->processo.picoCPU[0];

    fprintf(file , "P%d %d|", ptr->prox->processo.id, tempoAtual);

    ptr->prox->processo.jaFoi = 1;


    ptr = ptLista->prox;
    while (ptr != NULL) {
        ptr->processo.tempoDisponivel = ptr->processo.chegada + ptr->processo.picoCPU[0];
        ptr = ptr->prox;
    }
    

    while (processosRestantes > 0) {
        aux2 = 0;
        aux3 = 0;
        processosRestantes = 0;
        nextProc = NULL;
        ptr = ptLista;

        while (ptr != NULL) {
            if (ptr->processo.jaFoi == 0 && (nextProc == NULL || ptr->processo.picoCPU[0] < nextProc->processo.picoCPU[0])) {
                nextProc = ptr;
            }

            if (tempoAtual == ptr->processo.chegada && ptr->processo.jaFoi == 0) {
                nextProc = ptr;
            }

            if (ptr->processo.jaFoi == 0) {
                processosRestantes++;
            }

    
            ptr = ptr->prox;
        }


        if (nextProc != NULL) {
            ptr = ptLista;
            while (ptr != NULL) {
                if (tempoAtual + nextProc->processo.picoCPU[0] > ptr->processo.chegada && ptr->processo.jaFoi == 0 && nextProc->processo.prioridade < ptr->processo.prioridade) {
                    aux2 = (tempoAtual + nextProc->processo.picoCPU[0]) - ptr->processo.chegada;
                    nextProc->processo.interrompido = 1;
                    nextProc->processo.picoCPU[0] -= aux2;
                    tempoAtual += aux2;
                    fprintf (file , "P%d %d|" , nextProc->processo.id, tempoAtual);
                    aux3 = 1;
                    processosRestantes = 1;
                    nextProc->processo.interrompido = 1;
                    break;
                }
                ptr = ptr->prox;

            }

            if (aux3 != 1) {
                tempoAtual += nextProc->processo.picoCPU[0];
                nextProc->processo.jaFoi = 1;
                nextProc->processo.tempoDisponivel = tempoAtual + nextProc->processo.picoES[0];
                nextProc->processo.auxCPU = 1;
                fprintf(file , "P%d %d|", nextProc->processo.id, tempoAtual); 
            }

  
        }
    }

    ptr = ptLista;
     while (ptr != NULL) {
        if (ptr->processo.jaFoi == 1 && ptr->processo.auxCPU == 0) {
            ptr->processo.tempoDisponivel += ptr->processo.picoES[0];
        }
        ptr = ptr->prox;
    }


    ptr = ptLista;
    while (ptr != NULL) {
        ptr->processo.jaFoi = 0;
        ptr->processo.interrompido = 0;
        ptr->processo.auxCPU = 0;
        ptr = ptr->prox;
    }

    processosRestantes = 1;
    while (processosRestantes == 1) {
        processosRestantes = 0;
        ptr = ptLista;
        int nextProcTempoDisponivel = -1;
        int todosProcessadoresExecutados = 1;  
        int todosAcabaram = 1;  


        while (ptr != NULL) {
            if (ptr->processo.interrompido == 1) {
                tempoAtual += ptr->processo.picoCPU[0];
                fprintf (file , "P%d %d|", ptr->processo.id ,tempoAtual);
                ptr->processo.jaFoi = 1;
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[0];
                ptr->processo.interrompido = 0;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                    ptr->processo.acabou = 1;
                }
                break;
            }

            if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                ptr->processo.acabou = 1;
            }

            if (ptr->processo.acabou == 1) {
                ptr->processo.jaFoi = 1;
            }

            if (ptr->processo.tempoDisponivel > tempoAtual) {
                if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                    nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                }
                processosRestantes = 1;
            }
          
            if (ptr->processo.jaFoi == 0) {
                if (ptr->processo.tempoDisponivel < tempoAtual) {
                    if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 1) {
                            nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                            processosRestantes = 1;
                        }

                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 0) {
                            if (ptr->processo.tempoDisponivel < ptr->prox->processo.tempoDisponivel) {
                                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                                processosRestantes = 1;
                            } else {
                                nextProcTempoDisponivel = ptr->prox->processo.tempoDisponivel;
                                processosRestantes = 1;
                            }
                            
                        }
                    }
                }
                todosProcessadoresExecutados = 0;
            }

            if (ptr->processo.jaFoi == 0 && ptr->processo.tempoDisponivel == tempoAtual) {
                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                break;
            }


            if (ptr->processo.acabou == 0) {
                todosAcabaram = 0;
            }
            
            ptr = ptr->prox;
        }

        if (todosAcabaram) {
            break;
        }

        if (todosProcessadoresExecutados != 0) {
            ptr = ptLista;
            while (ptr != NULL) {
                ptr->processo.jaFoi = 0;
                ptr = ptr->prox;
            }
        }

        if (nextProcTempoDisponivel > tempoAtual) {
            fprintf(file , "*** %d|", nextProcTempoDisponivel);
            tempoAtual = nextProcTempoDisponivel;
        }

        ptr = ptLista;
        while (ptr != NULL) {
            if (ptr->processo.tempoDisponivel == tempoAtual && ptr->processo.jaFoi == 0) {
                fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }
                ptr->processo.jaFoi = 1;
                break;
            }

            if (ptr->processo.tempoDisponivel == nextProcTempoDisponivel && ptr->processo.jaFoi == 0) {
                fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                } 
                    
            
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                ptr->processo.jaFoi = 1;
                break;
            }
            ptr = ptr->prox;
        }
    }
    
    fprintf(file , "]\n");
    fprintf(file , "\n");


}

void RR(FILE *file, struct s_no *ptLista, int quantum) {
    struct s_no *ptr = ptLista;
    struct s_no *nextProc;
    int tempoAtual = 0;
    int processosRestantes = 1;
    int aux = 0;
    int total = 0;
    
    fprintf(file , "ROUND-ROBIN(q=%dms):\n", quantum);

    if (ptr != NULL) {
        tempoAtual = ptr->processo.chegada;
        fprintf(file , "%d[", tempoAtual);
    }

    while (ptr != NULL) {
        ptr->processo.auxPicoCPU = ptr->processo.picoCPU[0];
        total++;
        ptr = ptr->prox;
    }

    while (processosRestantes > 0) {
        processosRestantes = 0;
        nextProc = NULL;
        ptr = ptLista;

        while (ptr != NULL) {
            if (ptr->processo.auxCPU != 0 && ptr->processo.interrompido == 1 && ptr->processo.auxPicoCPU != ptr->processo.picoCPU[0] && ptr->prox != NULL && ptr->prox->processo.interrompido == 1) {
                ptr->processo.interrompido++;
                nextProc = ptr;
                processosRestantes = 1;
                break;
            }


            if (ptr->processo.jaFoi == 0 && ptr->processo.chegada <= tempoAtual) {
                if (nextProc == NULL || ptr->processo.chegada < nextProc->processo.chegada) {
                    nextProc = ptr;
                }
            }
            
            if (nextProc != NULL) {
                if (nextProc->processo.interrompido > 1) {
                    if (ptr->processo.jaFoi == 0 && ptr->processo.interrompido == 0 && ptr->processo.auxCPU == 0) {
                        nextProc = ptr;
                        break;
                    }

                }
            }

            
            

            if (ptr->processo.jaFoi == 0 && ptr->processo.chegada <= tempoAtual && ptr->processo.interrompido == 0) {
                if (nextProc == NULL || ptr->processo.chegada > nextProc->processo.chegada) {
                    if (ptr->processo.picoCPU[0] <= nextProc->processo.picoCPU[0]) {
                        nextProc = ptr;
                        break;
                    }
                }
            }

            if (ptr->processo.jaFoi == 0 && ptr->processo.chegada <= tempoAtual && ptr->processo.interrompido == 1) {
                if (ptr->processo.picoCPU[0] < nextProc->processo.picoCPU[0]) {
                    if (ptr->prox != NULL && ptr->prox->processo.acabou != 0) {
                        nextProc = ptr;
                        break;
                    }
                }
            }


            if (ptr->processo.jaFoi == 0) {
                processosRestantes++;
            }

            ptr = ptr->prox;
        }   
        

        if (nextProc != NULL) {
            if (aux == total - 1) {
                tempoAtual += nextProc->processo.picoCPU[0];
                nextProc->processo.jaFoi = 1;
                nextProc->processo.picoCPU[0] = 0;
                nextProc->processo.acabou = 1;
                nextProc->processo.interrompido = -1;
                nextProc->processo.auxCPU = -1;
                nextProc->processo.tempoDisponivel = tempoAtual + nextProc->processo.picoES[0];
                fprintf(file , "P%d %d|", nextProc->processo.id, tempoAtual);
                break;
            }

            if (nextProc->processo.picoCPU[0] > quantum) {
                tempoAtual += quantum;
                nextProc->processo.picoCPU[0] -= quantum;
                if (nextProc->processo.interrompido <= 1) {
                    nextProc->processo.interrompido = 1;
                }
                nextProc->processo.auxCPU++;
                fprintf(file , "P%d %d|", nextProc->processo.id, tempoAtual);

            } else {
                tempoAtual += nextProc->processo.picoCPU[0];
                nextProc->processo.jaFoi = 1;
                nextProc->processo.picoCPU[0] = 0;
                nextProc->processo.acabou = 1;
                nextProc->processo.interrompido = -1;
                nextProc->processo.auxCPU = -1;
                nextProc->processo.tempoDisponivel = tempoAtual + nextProc->processo.picoES[0];
                aux++;
                fprintf(file , "P%d %d|", nextProc->processo.id, tempoAtual);
            }
        } else {
            tempoAtual++;
        }
    }





    ptr = ptLista;
    while (ptr != NULL) {
        ptr->processo.jaFoi = 0;
        ptr->processo.interrompido = 0;
        ptr->processo.auxCPU = 0;
        ptr->processo.acabou = 0;
        ptr = ptr->prox;

    }
    

    processosRestantes = 1;
    while (processosRestantes == 1) {
        
        processosRestantes = 0;
        ptr = ptLista;
        int nextProcTempoDisponivel = -1;
        int todosProcessadoresExecutados = 1;  
        int todosAcabaram = 1; 

        while (ptr != NULL) {
            if (ptr->processo.auxCPU + 1 == ptr->processo.tamCPU - 1) {
                ptr->processo.acabou = 1;
            }

            if (ptr->processo.acabou == 1) {
                ptr->processo.jaFoi = 1;
            }


            if (ptr->processo.tempoDisponivel > tempoAtual) {
                if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                    nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                }
                processosRestantes = 1;
            }
          
            if (ptr->processo.jaFoi == 0) {
                if (ptr->processo.tempoDisponivel < tempoAtual) {
                    if (nextProcTempoDisponivel == -1 || ptr->processo.tempoDisponivel < nextProcTempoDisponivel) {
                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 1) {
                            nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                            processosRestantes = 1;
                        }

                        if (ptr->prox != NULL && ptr->prox->processo.jaFoi == 0) {
                            if (ptr->processo.tempoDisponivel < ptr->prox->processo.tempoDisponivel) {
                                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                                processosRestantes = 1;
                            } else {
                                nextProcTempoDisponivel = ptr->prox->processo.tempoDisponivel;
                                processosRestantes = 1;
                            }
                            
                        }
                    }
                }
                todosProcessadoresExecutados = 0;
            }

            if (ptr->processo.jaFoi == 0 && ptr->processo.tempoDisponivel == tempoAtual) {
                nextProcTempoDisponivel = ptr->processo.tempoDisponivel;
                break;
            }

            if (ptr->processo.acabou == 0) {
                todosAcabaram = 0;
            }
            
            ptr = ptr->prox;
        }

        if (todosAcabaram) {
            break;
        }

        if (todosProcessadoresExecutados != 0) {
            ptr = ptLista;
            while (ptr != NULL) {
                ptr->processo.jaFoi = 0;
                ptr = ptr->prox;
            }
        }

        if (nextProcTempoDisponivel > tempoAtual) {
            fprintf(file , "*** %d|", nextProcTempoDisponivel);
            tempoAtual = nextProcTempoDisponivel;
        }

        ptr = ptLista;
        while (ptr != NULL) {
            if (ptr->processo.tempoDisponivel == tempoAtual && ptr->processo.jaFoi == 0) {
                fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                }
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }
                ptr->processo.jaFoi = 1;
                break;
            }

            if (ptr->processo.tempoDisponivel == nextProcTempoDisponivel && ptr->processo.jaFoi == 0) {
                fprintf(file , "P%d %d|", ptr->processo.id, tempoAtual + ptr->processo.picoCPU[ptr->processo.contCPU]);
                tempoAtual += ptr->processo.picoCPU[ptr->processo.contCPU];
                ptr->processo.tempoDisponivel = tempoAtual + ptr->processo.picoES[ptr->processo.contES];

                ptr->processo.auxCPU++;
                ptr->processo.auxES++;

                if (ptr->processo.auxCPU < ptr->processo.tamCPU) {
                    ptr->processo.contCPU++;
                } 
                    
            
                if (ptr->processo.auxES < ptr->processo.tamES) {
                    ptr->processo.contES++;
                }

                ptr->processo.jaFoi = 1;
                break;
            }
            
            ptr = ptr->prox;
        }

        ptr = ptLista;
    }
    fprintf(file , "]\n");
    fprintf(file , "\n");
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <arquivo> <quantum> [-seq]\n", argv[0]);
        return 1;
    }

    int mostrarSeq = 0; 


    if (argc == 4) {  
    if (strcmp(argv[3], "-seq") == 0) {  
        mostrarSeq = 1;  
    }
}

    const char* nomeArquivo = argv[1];
    struct s_no *lista = NULL;
    int quantum = atoi(argv[2]);


    if (mostrarSeq) {
        FILE *file = fopen("processos-out.txt", "w");
        if (file == NULL) {
            printf("Erro ao abrir o arquivo!\n");
            return 1;
        }

        fprintf (file , "-seq\n");
    } else {
        FILE *file = fopen("processos-out.txt", "w");
        if (file == NULL) {
            printf("Erro ao abrir o arquivo!\n");
            return 1;
        }

        leArquivo(&lista, nomeArquivo);
        FCFS(file, lista);

        limpaLista(&lista); 
        leArquivo(&lista, nomeArquivo);
        SJF(file, lista);

        limpaLista(&lista); 
        leArquivo(&lista, nomeArquivo);
        SRTF(file, lista);

        limpaLista(&lista); 
        leArquivo(&lista, nomeArquivo);
        PRIO_PRMP(file, lista);

        limpaLista(&lista); 
        leArquivo(&lista, nomeArquivo);
        RR(file, lista, quantum);

        fclose(file);
    }


    return 0;
}
