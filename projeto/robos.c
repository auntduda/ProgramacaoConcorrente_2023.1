// Aluna: Maria Eduarda Carvalho Santos

// Problema dos Robos: Este problema eh caracterizado por: exitem N robos em uma esteira de producao de uma fabrica.
// A configuracao da fabrica exige que os robos estejam dispostos de tal maneira que seja possivel alocar uma bancada de 3 instrumentos entre 3 robos consecultivos.
// O problema consiste em realizar o andamento de um produto na esteira de producao, de tal modo que um produto entra de um lado, passa por n robos, e eh entregue ao final do processamento
// Cada robo dispoe de um machado, um martelo e um laco para realizar algum tipo de operacao sob o produto, e uma vez operado sobre, o produto segue para o proximo robo da esteira.
// Cada produto eh caracteristico, e pode aceitar ate 3 ferramentas para se operar: alguns produtos aceitam apenas o machado, outros apenas o laco, e alguns produtos aceitam um machado e um martelo como
// ferramenta de processamento

// Para resolver este problema de gerenciamento, serao usados - da pra usar semaforo aqui?

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "math.h"
#include "pthread.h"
#include "unistd.h"
//#include "semaphore.h"

pthread_mutex_t lock_bancada = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_martelo = PTHREAD_COND_INITIALIZER;

#define BANCADAS 5
#define ROBOS ((2*BANCADAS)+1)
#define PRODUTOS 13

//em TESE, eu queria criar uma lista dessa struct, pra representar cada bancada de ferramentas que tem entre 3 robos
typedef struct bancada{
    int martelo;
    int machado;
    int laco;
    char idMartelo;
    char idMachado;
    char idLaco;
} bancada;

typedef struct produto{
    int tipo;
}produtos;

bancada b[BANCADAS];
pthread_t robos[ROBOS];
produtos p[PRODUTOS];

void usaFerramenta(int idRobo, int idBancada, bool podeUsarDuas, int be, int bd, char ferramenta)
{
    // maracutaia pessoal que fiz pra pegar o nome do caraio da ferramenta da bancada pra modularizar este cacete 
    
    // char *tool;

    // switch (ferramenta){
    //     case 'm':
    //         *tool = "martelo";
    //         break;
    //     case 'h':
    //         *tool = "machado";
    //         break;
    //     case 'l':
    //         *tool = "laco";
    //         break;
    // }

    char tool[] = "ferramenta tal";

    if(podeUsarDuas)
    {
        //esse if aqui eh pra quando o robo so pode usar 1 bancada

        while(b[idBancada].martelo<=0)
        {
            // se nao tiver martelo na bancada, ele vai ter que esperar
            pthread_cond_wait(&cond_martelo, &lock_bancada);
        }
        // se algum robo passar daqui, eh porque o martelo voltou pra bancada

        if(b[idBancada].martelo>0)
        {
            b[idBancada].martelo--;

            printf("O robo %d pegou o %s da bancada %d\n", idRobo, tool, idBancada);
            
            // usando o martelo no produto
            sleep(3);

            //devolvendo o martelo pra bancada
            b[idBancada].martelo++;

            printf("O robo %d devolveu o %s para a bancada %d\n", idRobo, tool, idBancada);
            
            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o martelo
            pthread_cond_broadcast(&cond_martelo);
        }
    }else
    {
        //esse else aqui eh pra quando o robo pode usar as duas bancadas

        while(b[be].martelo<=0 && b[bd].martelo<=0)
        {
            // se nao tiver martelo em nenhuma das duas bancadas, ele vai ter que esperar
            pthread_cond_wait(&cond_martelo, &lock_bancada);
        }
        // se algum robo passar daqui, eh porque tem pelo menos 1 martelo em alguma das bancadas perto dele

        if(b[be].martelo>0)
        {
            b[be].martelo--;

            printf("O robo %d pegou o %s da bancada %d\n", idRobo, tool, be);
            
            sleep(3);

            b[be].martelo++;

            printf("O robo %d devolveu o %s para a bancada %d\n", idRobo, tool, be);
            
            pthread_cond_broadcast(&cond_martelo);
        }
        else if(b[bd].martelo>0)
        {
            b[bd].martelo--;

            printf("O robo %d pegou o %s da bancada %d\n", idRobo, tool, bd);

            sleep(3);
            b[bd].martelo++;

            printf("O robo %d devolveu o %s para a bancada %d\n", idRobo, tool, bd);

            pthread_cond_broadcast(&cond_martelo);
        }
    }
}

void* esteira(void* id)
{
    int i = *((int*) id);

    for(int j=0; j<PRODUTOS; j++)
    {
        printf("O robo %d esta pegando o produto %d para mexer\nO produto %d eh to tipo %d\n", i, j, j, p[j].tipo);
        
        // socando um lock aqui pra garantir que a verificacao da bancada nao vai sair errada
        pthread_mutex_lock(&lock_bancada);

        int besq = i/2; int bdir = ceil((i-2)/2); bool flag=false; int acessivel=-1;

        // vendo qual bancada o robo pode acessar e marcando qual bancada ele pode acessar

        if((i*2)!=besq)
        {
            acessivel = bdir; flag=true;
        }else if(((i*2)+2)!=bdir)
        {
            acessivel = besq; flag=true;
        }
        
        switch (p[j].tipo){
            case 1: // so pode usar martelo
                usaFerramenta(i, acessivel, flag, besq, bdir, 'm');

                break;
            case 2: // so pode usar machado
                usaFerramenta(i, acessivel, flag, besq, bdir, 'h');

                break;
            case 3: // so pode usar laco
                usaFerramenta(i, acessivel, flag, besq, bdir, 'l');

                break;
            case 4: // so pode usar martelo e machado
                // no meu problema, a ordem de uso das ferramentas importa ou nao importa?
                
                printf("feature a ser implementada pra ferramenta %d\n", p[j].tipo);
                break;
            case 5: // so pode usar martelo e laco
                printf("feature a ser implementada pra ferramenta %d\n", p[j].tipo);
                break;
            case 6: // so pode usar machado e laco
                printf("feature a ser implementada pra ferramenta %d\n", p[j].tipo);
                break;
            case 7: // pode usar os 3
                printf("feature a ser implementada pra ferramenta %d\n", p[j].tipo);
                break;
            default:
                printf("mlk como conseguiste pegar um tipo fora de 7 pela funcao rand\n");
                printf("se ele entrar aqui em algum momento, eh porque a funcao rand falhou\n");
                break;
        }

        pthread_mutex_unlock(&lock_bancada);
    }

    printf("mano se alguem chegar aqui embaixo eu serei feliz\n");
}

int main(/*int argc, char* argv[]*/)
{

    for(int i=0; i<BANCADAS; i++)
    {
        b[i].laco = 1; b[i].martelo = 1; b[i].machado = 1; b[i].idMartelo = 'm'; b[i].idMachado = 'h'; b[i].idLaco = 'l';  
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
        // quando eu dou o join aqui, ele executa o que cada robo faz, mas ele espera o processo terminar
        //pthread_join(robos[i], NULL);
    }
    
    // eu ainda nao entendi muito bem pra que serve um join, entao vou deixar ele comentado ate ver que meu programa quebra sem ele
    for(int i=0; i<ROBOS; i++) pthread_join(robos[i], NULL);


    return 0;
}

