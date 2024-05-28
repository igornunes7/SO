#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int *vetor; //vetor para armazenar os números
int tam; //tamanho do vetor
int num_threads; //quantidade de threads
int sum; //somatório global

//criando struct para marcar de onde até onde no vetor a thread irá somar
struct thread_args {
    int inicio, fim;
    int *local_sum;
};

void *runner(void *param); //os threads chamam essa função

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <file name> <integer value>\n", argv[0]);
        return -1;
    }

    num_threads = atoi(argv[2]);

    struct stat statbuf;

    FILE *arq;
    arq = fopen(argv[1], "rb");

    if (arq == NULL) { //se houve erro na abertura
        printf("Problemas na abertura do arquivo\n");
        return 0;
    }

    fstat(fileno(arq), &statbuf);

    //pega o tamanho do arquivo
    tam = statbuf.st_size / sizeof(int);

    vetor = (int *)malloc(tam * sizeof(int));

    //armazena os valores no vetor
    fread(vetor, sizeof(int), tam, arq);

    fclose(arq);

    pthread_attr_t attr; //conjunto de atributos das thread

    //verifica se o número de threads é negativo
    if (num_threads < 0) {
        printf("Num de threads invalido\n");
        return 0;
    }

    //verifica se não há nenhuma thread adicional
    if (num_threads == 0) {
        for (int i = 0; i < tam; i++) {
            sum += vetor[i];
        }
        printf("sum = %d\n", sum);
        free(vetor);
        return 0;
    }

    pthread_t tid[num_threads]; //o identificador das thread
    struct thread_args index_threads[num_threads]; //argumentos para as threads
    int local_sums[num_threads]; //vetor para armazenar a soma local de cada thread

    //obtem os atributos default
    pthread_attr_init(&attr);

    if (num_threads > tam) {
        num_threads = tam;
    }

    //dividindo intervalo de soma das threads
    int tam_intervalo = tam / num_threads;

    for (int i = 0; i < num_threads; i++) {
		//define o índice de início do intervalo que a thread irá começar
        index_threads[i].inicio = i * tam_intervalo;

		//verifica se chegou ao final do vetor
		if (i == num_threads - 1) {
			index_threads[i].fim = tam;
			//verifica qual sera o fim do intervalo se nao chegou no final
		} else {
			index_threads[i].fim = (i + 1) * tam_intervalo;
		}
		index_threads[i].local_sum = &local_sums[i]; //passar o ponteiro para armazenar a soma local
        pthread_create(&tid[i], NULL, runner, &index_threads[i]);
    }
    

    for (int i = 0; i < num_threads; i++) {
        //espera a thread ser encerrada
        pthread_join(tid[i], NULL);
        sum += local_sums[i]; //acumula a soma local de cada thread
    }
    //printf ("num threads = %d\n" , num_threads);
    printf("sum = %d\n", sum);

    free(vetor);

    return 0;
}

//a thread assumirá o controle nessa função
void *runner(void *param) {
    struct thread_args *index_threads = (struct thread_args *)param;
    int local_sum = 0;

    for (int i = index_threads->inicio; i < index_threads->fim; i++) {
        local_sum += vetor[i];
    }

    *(index_threads->local_sum) = local_sum; //salva a soma local
    //printf ("sum local = %d\n" , local_sum);

    pthread_exit(0);
}
