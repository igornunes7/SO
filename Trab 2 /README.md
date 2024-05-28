Considere o problema de somar números inteiros armazenados em um arquivo binário. Resolva esse problema das seguintes formas:

.Através da execução de um único processo multithread, onde:  a thread principal realiza a leitura do arquivo e armazena os números em um vetor;
são criadas threads adicionais para calcular a soma de subvetores; a thread principal soma os resultados obtidos pelas threads adicionais e imprime o resultado final;


.Através da execução de processos monothreads cooperativos, onde: o processo inicial realiza a leitura do arquivo e armazena os números em um vetor;
são criados processos adicionais para calcular a soma de subvetores; o processo inicial soma os resultados obtidos pelo processos adicionais e imprime o resultado final.


Toda a comunicação entre threads/processos deve ocorrer via memória compartilhada. 
Em ambos os casos o programa receberá como primeiro argumento o nome do arquivo a ser lido e como segundo argumento a quantidade de threads/processos a serem criados. 
A quantidade de números inteiros armazenada no arquivo deve ser calculada considerando o tamanho do arquivo em bytes e a quantidade de bytes necessária para armazenar um número inteiro. 