Faça um programa em C que calcule a quantidade de erros de páginas para os seguintes algoritmos de substituição:

    FIFO;
    OPT;
    LRU.

Os argumentos de linha de comando do programa são, nessa ordem:
1) O tamanho em bytes de cada página;
2) O tamanho em bytes da memória disponível para alocação de páginas;
3) O nome de um arquivo contendo uma sequência de endereços acessados. Esse, no formato texto, conterá endereços separados por um espaço cujos valores variam 0 a 65.535. Em cada acesso será referenciado apenas um byte.

Ao final da execução devem ser apresentados na tela, para cada algoritmo, o número de erros de página acompanhado do percentual de erros em relação à quantidade de endereços acessados. Também deve ser gravado no arquivo texto erros.out os endereços e páginas que ocasionaram os erros de página para cada algoritmo de substituição.

O arquivo seq-20-paginas-10.txt é um exemplo de entrada com 123 endereços acessados. A execução do programa com páginas de tamanho 10 e tamanho de memória variando de 30 a 39 (3 quadros), irá produzir o arquivo erros.out a seguir, com taxa de erros de página de 12,19% para o FIFO,  7,31% para o OPT e 9,75% para o LRU.
