#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

int *vetor; //vetor para armazenar os números
int tam; //tamanho do vetor
int num_process; //quantidade de processos
int *sum; //ponteiro para a soma final

//criando struct para marcar de onde até onde no vetor o processo irá somar
struct process_args {
    int inicio, fim;
    int *local_sum;
};

void runner(struct process_args *param); //os processos chamam essa função

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <file name> <integer value>\n", argv[0]);
        return -1;
    }

    num_process = atoi(argv[2]);

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

    //criando memória compartilhada para o sum (criando o objeto)
    int shm_fd_sum = shm_open("/sum", O_CREAT | O_RDWR, 0666);
    if (shm_fd_sum == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    //fazendo o objeto ter o tamanho de bytes de sizeof(int)
    ftruncate(shm_fd_sum, sizeof(int));
    if (ftruncate(shm_fd_sum, sizeof(int)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    //mapeando o sum para o espaço de endereçamento do processo
    sum = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_sum, 0);

    *sum = 0;


    //criando memória compartilhada para os local_sums (criando o objeto)
    int shm_fd_local_sums = shm_open("/local_sums", O_CREAT | O_RDWR, 0666);
    if (shm_fd_local_sums == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    //fazendo o objeto ter o tamanho de bytes de num_process * sizeof(int)
    ftruncate(shm_fd_local_sums, num_process * sizeof(int));
    if (ftruncate(shm_fd_local_sums, num_process * sizeof(int)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    //mapeando o local_sums para o espaço de endereçamento do processo
    int *local_sums = (int *)mmap(0, num_process * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_local_sums, 0);


    for (int i = 0; i < num_process; i++) {
        local_sums[i] = 0;
    }



    //verifica se o número de processos é negativo
    if (num_process < 0) {
        printf("Num de processos invalido\n");
        return 0;
    }

    //verifica se não há nenhum processo adicional
    if (num_process == 0) {
        for (int i = 0; i < tam; i++) {
            *sum += vetor[i];
        }
        printf("sum = %d\n", *sum);

        //fechando o mapa e a memória compartilhada
        munmap("/sum", sizeof(int));
        shm_unlink("/sum");
        munmap("/local_sums", num_process * sizeof(int));
        shm_unlink("/local_sums");

        free(vetor);

        return 0;
    }

    if (num_process > tam) {
        num_process = tam;
    }

    //dividindo intervalo de soma dos processos
    int tam_intervalo = tam / num_process;

    //printf("tam = %d\n", tam);
    //printf("num_process = %d\n", num_process);

    int status;

    struct process_args index_process[num_process];
    pid_t pid[num_process]; // array para armazenar os pid dos processos filhos

    //criando processos filhos
    for (int i = 0; i < num_process; i++) {
        pid[i] = fork(); // atribuição do pid
        if (pid[i] == 0) {
            //processo filho
            index_process[i].inicio = i * tam_intervalo;
            if (i == num_process - 1) {
                index_process[i].fim = tam;
            } else {
                index_process[i].fim = (i + 1) * tam_intervalo;
            }
            index_process[i].local_sum = &local_sums[i];
            runner(&index_process[i]); // chamada da função runner
            exit(0);
        } else if (pid[i] < 0) {
            //erro na criação do processo
            fprintf(stderr, "Erro ao criar processo filho\n");
            return -1;
        }
    }

    //esperando todos os processos filhos terminarem
    for (int i = 0; i < num_process; i++) {
        waitpid(pid[i], &status, 0);
    }

    //somando todos os resultados parciais
    for (int i = 0; i < num_process; i++) {
        *sum += local_sums[i];
    }

    printf("sum = %d\n", *sum);

    //fechando o mapa e a memória compartilhada
    munmap("/sum", sizeof(int));
    shm_unlink("/sum");
    munmap("/local_sums", num_process * sizeof(int));
    shm_unlink("/local_sums");

    free(vetor);

    return 0;
}

void runner(struct process_args *param) {
    //acessar os parâmetros dos índices inicio e fim
    struct process_args *index_process = (struct process_args *)param;
    int local_sum = 0;

    for (int i = index_process->inicio; i < index_process->fim; i++) {
        local_sum += vetor[i];
    }

    //printf("(RUNNER)local sum = %d\n", local_sum);

    *(index_process->local_sum) = local_sum; //salva a soma local

    //printf("(RUNNER)local sum passada = %d\n", *index_process->local_sum);
}
