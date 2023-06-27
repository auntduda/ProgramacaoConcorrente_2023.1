// Aluna: Maria Eduarda Carvalho Santos

// Problema dos Robos: Este problema eh caracterizado por: exitem N robos em uma esteira de producao de uma fabrica.
// A configuracao da fabrica exige que os robos estejam dispostos de tal maneira que seja possivel alocar uma bancada de 3 instrumentos entre 3 robos consecultivos.
// O problema consiste em realizar o andamento de um produto na esteira de producao, de tal modo que um produto entra de um lado, passa por n robos, e eh entregue ao final do processamento
// Cada robo dispoe de um machado, um martelo e um laco para realizar algum tipo de operacao sob o produto, e uma vez operado sobre, o produto segue para o proximo robo da esteira.
// Cada produto eh caracteristico, e pode aceitar ate 3 ferramentas para se operar: alguns produtos aceitam apenas o machado, outros apenas o laco, e alguns produtos aceitam um machado e um martelo como
// ferramenta de processamento

// Para resolver este problema de gerenciamento, serao usados - da pra usar semaforo aqui?

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"
//#include "semaphore.h"

#define BANCADAS 5
#define ROBOS ((2*BANCADAS)+1)
#define PRODUTOS 13

//em TESE, eu queria criar uma lista dessa struct, pra representar cada bancada de ferramentas que tem entre 3 robos
typedef struct bancada{
    int martelo;
    int machado;
    int laco;
} bancada;

typedef struct produto{
    int tipo;
}produtos;

bancada b[BANCADAS];
pthread_t robos[ROBOS];
produtos p[PRODUTOS];

void* esteira(void* id)
{
    int i = *((int*) id);

    for(int j=0; j<PRODUTOS; j++)
    {
        printf("O robo %d esta pegando o produto %d para mexer\nO produto %d eh to tipo %d\n", i, j, j, p[j].tipo);
        sleep(3);
    }

}

// adicionando um comentario aqui pra commitar as mudancas de verdade

int main(/*int argc, char* argv[]*/)
{

    for(int i=0; i<BANCADAS; i++)
    {
        b[i].laco = 1; b[i].martelo = 1; b[i].machado = 1;
        // dependendo de onde eu instancio isso aqui, robos diferentes pegam um instrumento da bancada
    }

    // classificando os produtos por tipos
    for(int i=0; i<PRODUTOS; i++) p[i].tipo = rand()%7;

    for(int i=0; i<ROBOS; i++)
    {
        int* id;
        id = (int*) malloc(sizeof(int));
        *id = i;

        pthread_create(&robos[i], NULL, &esteira, (void*) (id));
        pthread_join(robos[i], NULL);
    }
    
    // eu ainda nao entendi muito bem pra que serve um join, entao vou deixar ele comentado ate ver que meu programa quebra sem ele
    //for(int i=0; i<ROBOS; i++) pthread_join(robos[i], NULL);


    return 0;
}

