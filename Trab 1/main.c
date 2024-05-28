#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>


#define MAX_LINE 200
#define MAX_CHILDREN 128
#define MAX_argv 10


//nó da arvore
struct pstree_node {
	char name[128];
	pid_t pid;
	pid_t ppid;
	struct pstree_node * parent;
	struct pstree_node * children[MAX_CHILDREN];
	struct pstree_node * next;
};


int* lerNumerosProc(int* num_numeros) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    int max_numeros = 2000;  //maximo de entradas no /proc
    int* numeros = malloc(max_numeros * sizeof(int));  

    if (numeros == NULL) {
        perror("Erro");
        *num_numeros = 0;
        return NULL;
    }

    //abrir o diretório /proc
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Erro ao abrir diretório /proc");
        free(numeros);  //liberar memória alocada antes de retornar
        *num_numeros = 0;
        return NULL;
    }

    //eer cada entrada no diretório /proc
    while ((entry = readdir(dir)) != NULL) {
        //verificar se o nome da entrada é um número
        if (entry->d_type == DT_DIR) {
            char *ptr;
            long pid = strtol(entry->d_name, &ptr, 10);  //converter nome para número

            //se nao der erro
            if (*ptr == '\0') {
                //armazenar o número
                if (count < max_numeros) {
                    numeros[count] = (int)pid;
                    count++;
                } else {
                    break;  
                }
            }
        }
    }

    closedir(dir);
    *num_numeros = count; 

    return numeros;  
}


char* find_ppid(char *pid_number) {
    char filename[MAX_LINE];
    char line[MAX_LINE];
    char *token[10];
    char procname[MAX_LINE];
    static char ppid[MAX_LINE];
    char pid[MAX_LINE];
    int i = 0;

    //caminho para o diretório do processo
    sprintf(filename, "/proc/%s/stat", pid_number);


    //abrir o arquivo stat para leitura
    FILE *p_file = fopen(filename, "r");
    if (p_file == NULL) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }
    
    //ler a linha do arquivo stat
    if (fgets(line, sizeof(line), p_file) != NULL) {
        token[i] = strtok(line, " ");
        while (token[i] != NULL && i < 10) {
            i++;
            token[i] = strtok(NULL, " ");
        }
    }

    //token[2] == S (nao composto)
    //token[3] == S (composto)


    //strcpy(pid , token[0]);
    //printf ("PID: %s\n\n", pid);
    strcpy(ppid , token[3]);
    //printf ("PPID STRING: %s\n" , ppid);

    /* 
    if (strcmp(token[3], "S") == 0) {
        strcpy(procname , token[1]);
        strcat(procname , token[2]);
    } else {
        strcpy(procname , token[1]);

    }
    printf ("NAME: %s\n\n" , procname);
    */
    //fechar o arquivo
    fclose(p_file);

    return ppid;
}


void verify_ppid (char *ppid) {
    int conv= atoi(ppid);
    int* pid_list;
    int num_pids;
    int maior_list[MAX_LINE];
    int i, cont;
    char pid_str[MAX_LINE];
    char* children[MAX_LINE];
    int aux = 0;

    cont = 0;

    pid_list = lerNumerosProc(&num_pids);
    if (pid_list == NULL) {
        printf("Erro\n");
        return;
    }

    for (i = 0; i < num_pids; i++) {
        if (pid_list[i] > conv) {
            maior_list[cont] = pid_list[i];
            cont++;
        }
    }
    int ppid_int[cont];
    for (i = 0; i < cont; i++) {
        sprintf(pid_str, "%d", maior_list[i]);
        //printf ("PID: %s\n", pid_str);
        ppid_int[i] = atoi(find_ppid(pid_str));  //chamar find_ppid com o PID em formato de string
        //printf ("PPID INTEIRO: %d\n\n" , pid_int[i]);
        //printf("PID[%d]: %s, PPID[%d]: %d\n", i, pid_str, i, ppid_int[i]);
    }   
    int counter = 0;
    for (i = 0; i < cont; i++) {
        if (ppid_int[i] == conv) {
            //printf ("ENTROU\n\n");
            counter++;
            aux++;
        }
    }

    printf ("O PPID %d TEM %d FILHOS\n", conv, counter); 


}

char *get_user() {
    uid_t uid = geteuid();  //obtem o UID do usuário
    struct passwd *pw = getpwuid(uid);  //obtem informações do usuário pelo UID

    return pw->pw_name;  //retorna o nome de usuário
}


void printEntry (char * dir, char * user) {
    printf ("\033[1;32m%s@\033[0m:\033[1;31m~%s\033[0m$ " , user, getcwd(NULL, 200));
    //fflush(stdout);
}

int main() {
    int status;
    char tree_pid[10];
    char directory[200];
    char line[MAX_LINE];
    char * argv[MAX_argv];
    char *username = get_user();
    int* numeros;
    int num_numeros;


    while (1) {
        printEntry(directory, username);
        fgets(line, MAX_LINE, stdin);

        char cmd[40] = "/usr/bin/";

        //tokenizar
        int i = 0;
        
        argv[i] = strtok(line, " \n");
        while (argv[i] != NULL && i < MAX_argv - 1) {
            i++;
            argv[i] = strtok(NULL, " \n");
        }
        argv[i] = NULL;


        if (i == 0) {
            continue;
        }

        if (strcmp(argv[0] , "&") == 0) {
            continue;
        }


        //comando para usar CD
        if (strcmp(argv[0], "cd") == 0) {
            if (argv[1] != NULL) {
                if (chdir(argv[1]) != 0) {
                    perror("chdir failed");
                }
            }
            continue;  
        }

        //comando de saida
        else if (strcmp(argv[0], "exit") == 0) {
            exit(0);
        }


        if (strcmp(argv[0], "tree") == 0) {
            if (argv[1] != NULL) {
                //numeros = lerNumerosProc(&num_numeros);
                verify_ppid(argv[1]);
                //find_ppid(argv[1]);
                //printf ("%d\n%d\n%d\n", numeros[23], numeros[234], numeros[23]);

            }
            continue;
        }

        strcat(cmd, argv[0]);

        //criando filho
        pid_t pid = fork();
        if (pid < 0) {
            //se der erro
            perror("fork failed!");
            exit(1);
        }   
        else if (pid == 0) {
            //processo filho
            execve (cmd, argv, __environ);
            perror("execve failed!");
            //emset(cmd, '\0' , 40);
            //memset(argv, '\0', 10);
            //memset(line, '\0' , 200);
            exit(1);
        }
        else {
            // processo pai
            if (strcmp(argv[i - 1] , "&") == 0) {
                continue;
            } else {
                waitpid(pid, &status, 0);
            }
        }
    }
    return 0;
}
