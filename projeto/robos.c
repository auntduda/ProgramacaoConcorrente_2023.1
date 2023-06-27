// Aluna: Maria Eduarda Carvalho Santos

// Problema dos Robos: Este problema eh caracterizado por: exitem N robos em uma esteira de producao de uma fabrica.
// A configuracao da fabrica exige que os robos estejam dispostos de tal maneira que seja possivel alocar uma bancada de 3 instrumentos entre 3 robos consecultivos.
// O problema consiste em realizar o andamento de um produto na esteira de producao, de tal modo que um produto entra de um lado, passa por n robos, e eh entregue ao final do processamento
// Cada robo dispoe de um machado, um martelo e um laco para realizar algum tipo de operacao sob o produto, e uma vez operado sobre, o produto segue para o proximo robo da esteira.
// Cada produto eh caracteristico, e pode aceitar ate 3 ferramentas para se operar: alguns produtos aceitam apenas o machado, outros apenas o laco, e alguns produtos aceitam um machado e um martelo como
// ferramenta de processamento

// Para resolver este problema de gerenciamento, serao usados locks de controle de fluxo e variaveis condicionais para garantir integridade

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "math.h"
#include "pthread.h"
#include "unistd.h"
//#include "semaphore.h"


// rindo aqui
pthread_mutex_t lock_bancada = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_martelo = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_machado = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_laco = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_marteloMachado = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_marteloLaco = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_machadoLaco = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_marteloMachadoLaco = PTHREAD_COND_INITIALIZER;

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
    // essa funcao aqui era pra me ajudar a modularizar esse codigo pra debugar ele posteriormente, mas ja que eu sou um MAMACO, depois eu refatoro isso daqui
}

void* esteira(void* id)
{
    int i = *((int*) id);

    for(int j=0; j<PRODUTOS; j++)
    {
        printf("[Se liga!] O robo %d esta pegando o produto %d para mexer\n[ATENCAO!] O produto %d eh to tipo %d\n", i, j, j, p[j].tipo);
        
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
        
        // aqui comeca a merda
        // modularizar isso aqui depois pra debugar - agora vai na marra mesmo
        switch (p[j].tipo){
            case 1: // so pode usar martelo
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].martelo<=0)
                    {
                        // se nao tiver martelo na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_martelo, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o martelo voltou pra bancada

                    if(b[acessivel].martelo>0)
                    {
                        b[acessivel].martelo--;

                        printf(">> O robo %d pegou o martelo da bancada %d\n", i, acessivel);
                        
                        // usando o martelo no produto
                        sleep(3);

                        //devolvendo o martelo pra bancada
                        b[acessivel].martelo++;

                        printf("> O robo %d devolveu o martelo para a bancada %d\n", i, acessivel);
                        
                        // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o martelo
                        pthread_cond_broadcast(&cond_martelo);
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while(b[besq].martelo<=0 && b[bdir].martelo<=0)
                    {
                        // se nao tiver martelo em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_martelo, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 martelo em alguma das bancadas perto dele

                    if(b[besq].martelo>0)
                    {
                        b[besq].martelo--;

                        printf(">> O robo %d pegou o martelo da bancada %d\n", i, besq);
                        
                        sleep(3);

                        b[besq].martelo++;

                        printf("> O robo %d devolveu o martelo para a bancada %d\n", i, besq);
                        
                        pthread_cond_broadcast(&cond_martelo);
                    }
                    else if(b[bdir].martelo>0)
                    {
                        b[bdir].martelo--;

                        printf(">> O robo %d pegou o martelo da bancada %d\n", i, bdir);

                        sleep(3);
                        b[bdir].martelo++;

                        printf("> O robo %d devolveu o martelo para a bancada %d\n", i, bdir);

                        pthread_cond_broadcast(&cond_martelo);
                    }
                }

                break;
            case 2: // so pode usar machado
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].machado<=0)
                    {
                        // se nao tiver machado na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um machado na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_machado, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o machado voltou pra bancada

                    if(b[acessivel].machado>0)
                    {
                        b[acessivel].machado--;

                        printf(">> O robo %d pegou o machado da bancada %d\n", i, acessivel);
                        
                        // usando o machado no produto
                        sleep(2);

                        //devolvendo o machado pra bancada
                        b[acessivel].machado++;

                        printf("> O robo %d devolveu o machado para a bancada %d\n", i, acessivel);
                        
                        // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o machado
                        pthread_cond_broadcast(&cond_machado);
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while(b[besq].machado<=0 && b[bdir].machado<=0)
                    {
                        // se nao tiver machado em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um machado na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_machado, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 machado em alguma das bancadas perto dele

                    if(b[besq].machado>0)
                    {
                        b[besq].machado--;

                        printf(">> O robo %d pegou o machado da bancada %d\n", i, besq);
                        
                        sleep(2);

                        b[besq].machado++;

                        printf("> O robo %d devolveu o machado para a bancada %d\n", i, besq);
                        
                        pthread_cond_broadcast(&cond_machado);
                    }
                    else if(b[bdir].machado>0)
                    {
                        b[bdir].machado--;

                        printf(">> O robo %d pegou o machado da bancada %d\n", i, bdir);

                        sleep(2);
                        b[bdir].machado++;

                        printf("> O robo %d devolveu o machado para a bancada %d\n", i, bdir);

                        pthread_cond_broadcast(&cond_machado);
                    }
                }

                break;
            case 3: // so pode usar laco
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].laco<=0)
                    {
                        // se nao tiver laco na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um laco na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_laco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o laco voltou pra bancada

                    if(b[acessivel].laco>0)
                    {
                        b[acessivel].laco--;

                        printf(">> O robo %d pegou o laco da bancada %d\n", i, acessivel);
                        
                        // usando o laco no produto
                        sleep(4);

                        //devolvendo o laco pra bancada
                        b[acessivel].laco++;

                        printf("> O robo %d devolveu o laco para a bancada %d\n", i, acessivel);
                        
                        // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o laco
                        pthread_cond_broadcast(&cond_laco);
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while(b[besq].laco<=0 && b[bdir].laco<=0)
                    {
                        // se nao tiver laco em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um laco na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_laco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 laco em alguma das bancadas perto dele

                    if(b[besq].laco>0)
                    {
                        b[besq].laco--;

                        printf(">> O robo %d pegou o laco da bancada %d\n", i, besq);
                        
                        sleep(4);

                        b[besq].laco++;

                        printf("> O robo %d devolveu o laco para a bancada %d\n", i, besq);
                        
                        pthread_cond_broadcast(&cond_laco);
                    }
                    else if(b[bdir].laco>0)
                    {
                        b[bdir].laco--;

                        printf(">> O robo %d pegou o laco da bancada %d\n", i, bdir);

                        sleep(4);
                        b[bdir].laco++;

                        printf("> O robo %d devolveu o laco para a bancada %d\n", i, bdir);

                        pthread_cond_broadcast(&cond_laco);
                    }
                }

                break;
            case 4: // so pode usar martelo e machado
                // no meu problema, a ordem de uso das ferramentas importa ou nao importa? R: nao importa, eu so quero que ele use as duas ferramentas
                
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].martelo<=0 || b[acessivel].machado<=0)
                    {
                        // se nao tiver martelo na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo ou por um machado na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_marteloMachado, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o martelo ou o machado voltaram pra bancada

                    bool pegouUmaFerramenta=false; // essa flag aqui eh feita pra me dizer se o robo pegou uma ferramenta e agora tem que pegar a outra
                    int ferrUsadas=2; // com a ajuda da flag de cima, essa contadora me ajuda a ver se o robo usou as duas ferramentas

                    while(ferrUsadas)
                    {
                        if(b[acessivel].martelo>0 && !pegouUmaFerramenta)
                        {
                            b[acessivel].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, acessivel);
                            
                            // usando o martelo no produto
                            sleep(3);

                            //devolvendo o martelo pra bancada
                            b[acessivel].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o martelo
                            pthread_cond_broadcast(&cond_marteloMachado); ferrUsadas--; pegouUmaFerramenta=true;
                        }else if(b[acessivel].machado>0 && pegouUmaFerramenta)
                        {
                            b[acessivel].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, acessivel);
                            
                            // usando o machado no produto
                            sleep(2);

                            //devolvendo o machado pra bancada
                            b[acessivel].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o machado
                            pthread_cond_broadcast(&cond_marteloMachado); ferrUsadas--;
                        }
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while((b[besq].martelo<=0 && b[bdir].martelo<=0) || (b[besq].machado<=0 && b[bdir].machado<=0))
                    {
                        // se nao tiver martelo em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo ou por um machado na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_martelo, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 martelo ou pelo menos 1 machado em alguma das bancadas perto dele

                    bool pegouUmaFerramenta=false;
                    int ferrUsadas=2;

                    while(ferrUsadas)
                    {
                        if(b[besq].martelo>0 && !pegouUmaFerramenta)
                        {
                            b[besq].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, besq);
                            
                            sleep(3);

                            b[besq].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, besq);
                            
                            pthread_cond_broadcast(&cond_marteloMachado); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[besq].machado>0 && pegouUmaFerramenta)
                        {
                            b[besq].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, besq);

                            sleep(2);
                            b[besq].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, besq);

                            pthread_cond_broadcast(&cond_marteloMachado); ferrUsadas--;
                        }else if(b[bdir].martelo>0 && !pegouUmaFerramenta)
                        {
                            b[bdir].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, bdir);
                            
                            sleep(3);

                            b[bdir].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, bdir);
                            
                            pthread_cond_broadcast(&cond_marteloMachado); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[bdir].machado>0 && pegouUmaFerramenta)
                        {
                            b[bdir].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, bdir);

                            sleep(2);
                            b[bdir].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, bdir);

                            pthread_cond_broadcast(&cond_marteloMachado); ferrUsadas--;
                        }
                    }
                }

                break;
            case 5: // so pode usar martelo e laco
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].martelo<=0 || b[acessivel].laco<=0)
                    {
                        // se nao tiver martelo na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo ou por um laco na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_marteloLaco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o martelo ou o laco voltaram pra bancada

                    bool pegouUmaFerramenta=false; // essa flag aqui eh feita pra me dizer se o robo pegou uma ferramenta e agora tem que pegar a outra
                    int ferrUsadas=2; // com a ajuda da flag de cima, essa contadora me ajuda a ver se o robo usou as duas ferramentas

                    while(ferrUsadas)
                    {
                        if(b[acessivel].martelo>0 && !pegouUmaFerramenta)
                        {
                            b[acessivel].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, acessivel);
                            
                            // usando o martelo no produto
                            sleep(3);

                            //devolvendo o martelo pra bancada
                            b[acessivel].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o martelo
                            pthread_cond_broadcast(&cond_marteloLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }else if(b[acessivel].laco>0 && pegouUmaFerramenta)
                        {
                            b[acessivel].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, acessivel);
                            
                            // usando o laco no produto
                            sleep(4);

                            //devolvendo o laco pra bancada
                            b[acessivel].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o laco
                            pthread_cond_broadcast(&cond_marteloLaco); ferrUsadas--;
                        }
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while((b[besq].martelo<=0 && b[bdir].martelo<=0) || (b[besq].laco<=0 && b[bdir].laco<=0))
                    {
                        // se nao tiver martelo em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo ou por um laco na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_martelo, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 martelo ou pelo menos 1 laco em alguma das bancadas perto dele

                    bool pegouUmaFerramenta=false;
                    int ferrUsadas=2;

                    while(ferrUsadas)
                    {
                        if(b[besq].martelo>0 && !pegouUmaFerramenta)
                        {
                            b[besq].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, besq);
                            
                            sleep(3);

                            b[besq].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, besq);
                            
                            pthread_cond_broadcast(&cond_marteloLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[besq].laco>0 && pegouUmaFerramenta)
                        {
                            b[besq].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, besq);

                            sleep(4);
                            b[besq].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, besq);

                            pthread_cond_broadcast(&cond_marteloLaco); ferrUsadas--;
                        }else if(b[bdir].martelo>0 && !pegouUmaFerramenta)
                        {
                            b[bdir].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, bdir);
                            
                            sleep(3);

                            b[bdir].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, bdir);
                            
                            pthread_cond_broadcast(&cond_marteloLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[bdir].laco>0 && pegouUmaFerramenta)
                        {
                            b[bdir].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, bdir);

                            sleep(4);
                            b[bdir].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, bdir);

                            pthread_cond_broadcast(&cond_marteloLaco); ferrUsadas--;
                        }
                    }
                }

                break;
            case 6: // so pode usar machado e laco
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].laco<=0 || b[acessivel].machado<=0)
                    {
                        // se nao tiver laco na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um laco ou por um machado na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_machadoLaco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o laco ou o machado voltaram pra bancada

                    bool pegouUmaFerramenta=false; // essa flag aqui eh feita pra me dizer se o robo pegou uma ferramenta e agora tem que pegar a outra
                    int ferrUsadas=2; // com a ajuda da flag de cima, essa contadora me ajuda a ver se o robo usou as duas ferramentas

                    while(ferrUsadas)
                    {
                        if(b[acessivel].laco>0 && !pegouUmaFerramenta)
                        {
                            b[acessivel].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, acessivel);
                            
                            // usando o laco no produto
                            sleep(4);

                            //devolvendo o laco pra bancada
                            b[acessivel].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o laco
                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }else if(b[acessivel].machado>0 && pegouUmaFerramenta)
                        {
                            b[acessivel].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, acessivel);
                            
                            // usando o machado no produto
                            sleep(2);

                            //devolvendo o machado pra bancada
                            b[acessivel].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o machado
                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--;
                        }
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while((b[besq].laco<=0 && b[bdir].laco<=0) || (b[besq].machado<=0 && b[bdir].machado<=0))
                    {
                        // se nao tiver laco em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um laco ou por um machado na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_laco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 laco ou pelo menos 1 machado em alguma das bancadas perto dele

                    bool pegouUmaFerramenta=false;
                    int ferrUsadas=2;

                    while(ferrUsadas)
                    {
                        if(b[besq].laco>0 && !pegouUmaFerramenta)
                        {
                            b[besq].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, besq);
                            
                            sleep(4);

                            b[besq].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, besq);
                            
                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[besq].machado>0 && pegouUmaFerramenta)
                        {
                            b[besq].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, besq);

                            sleep(2);
                            b[besq].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, besq);

                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--;
                        }else if(b[bdir].laco>0 && !pegouUmaFerramenta)
                        {
                            b[bdir].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, bdir);
                            
                            sleep(4);

                            b[bdir].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, bdir);
                            
                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[bdir].machado>0 && pegouUmaFerramenta)
                        {
                            b[bdir].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, bdir);

                            sleep(2);
                            b[bdir].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, bdir);

                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--;
                        }
                    }
                }

                break;
            case 7: // pode usar os 3
                if(flag)
                {
                    //esse if aqui eh pra quando o robo so pode usar 1 bancada

                    while(b[acessivel].martelo<=0 || b[acessivel].laco<=0 || b[acessivel].machado<=0)
                    {
                        // se nao tiver martelo nem machado nem laco na bancada, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo ou por um laco ou por um machado na bancada %d !!\n", i, acessivel);
                        pthread_cond_wait(&cond_marteloMachadoLaco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque o martelo ou o laco ou o machado voltaram pra bancada

                    bool pegouUmaFerramenta=false; bool pegouDuasFerramentas=false; // essas flags aqui sao pra eu controlar quais ferramentas ja forma usadas pra eu nao usar a mesma ferramenta de novo e cagar com minha logica de problema
                    int ferrUsadas=3; // com a ajuda da flag de cima, essa contadora me ajuda a ver se o robo usou as tres ferramentas

                    while(ferrUsadas) // eu meio que to dando a ordem em que ele pega as ferramentas, QUE MERDAAAAAAAAAAAAAAAAAAAAAA
                    {
                        if(b[acessivel].martelo>0 && (!pegouUmaFerramenta && !pegouDuasFerramentas))
                        {
                            b[acessivel].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, acessivel);
                            
                            // usando o martelo no produto
                            sleep(3);

                            //devolvendo o martelo pra bancada
                            b[acessivel].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o martelo
                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }else if(b[acessivel].laco>0 && (pegouUmaFerramenta && !pegouDuasFerramentas))
                        {
                            b[acessivel].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, acessivel);
                            
                            // usando o laco no produto
                            sleep(4);

                            //devolvendo o laco pra bancada
                            b[acessivel].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o laco
                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--; pegouDuasFerramentas=true;
                        }else if(b[acessivel].machado>0 && (pegouUmaFerramenta && pegouDuasFerramentas))
                        {
                            b[acessivel].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, acessivel);
                            
                            // usando o machado no produto
                            sleep(2);

                            //devolvendo o machado pra bancada
                            b[acessivel].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, acessivel);
                            
                            // aqui ele deveria dar um broadcast pra todo mundo que ta esperando o machado
                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--;
                        }
                    }
                }else
                {
                    //esse else aqui eh pra quando o robo pode usar as duas bancadas

                    while((b[besq].martelo<=0 && b[bdir].martelo<=0) || (b[besq].laco<=0 && b[bdir].laco<=0) || (b[besq].machado<=0 && b[bdir].machado<=0))
                    {
                        // se nao tiver martelo nem um laco e nem um machado em nenhuma das duas bancadas, ele vai ter que esperar
                        printf("!! O robo %d esta esperando por um martelo ou por um laco ou por um machado na bancada %d e na bancada %d !!\n", i, besq, bdir);
                        pthread_cond_wait(&cond_marteloMachadoLaco, &lock_bancada);
                    }
                    // se algum robo passar daqui, eh porque tem pelo menos 1 martelo ou pelo menos 1 laco ou pelo menos 1 machado em alguma das bancadas perto dele

                    bool pegouUmaFerramenta=false; bool pegouDuasFerramentas=false;
                    int ferrUsadas=3;

                    while(ferrUsadas)
                    {
                        if(b[besq].martelo>0 && (!pegouUmaFerramenta && !pegouDuasFerramentas))
                        {
                            b[besq].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, besq);
                            
                            sleep(3);

                            b[besq].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, besq);
                            
                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[besq].laco>0 && (pegouUmaFerramenta && !pegouDuasFerramentas))
                        {
                            b[besq].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, besq);

                            sleep(4);
                            b[besq].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, besq);

                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--; pegouDuasFerramentas=true;
                        }else if(b[besq].machado>0 && (pegouUmaFerramenta && pegouDuasFerramentas))
                        {
                            b[besq].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, besq);

                            sleep(2);
                            b[besq].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, besq);

                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--;
                        }else if(b[bdir].martelo>0 && (!pegouUmaFerramenta && !pegouDuasFerramentas))
                        {
                            b[bdir].martelo--;

                            printf(">> O robo %d pegou o martelo da bancada %d\n", i, bdir);
                            
                            sleep(3);

                            b[bdir].martelo++;

                            printf("> O robo %d devolveu o martelo para a bancada %d\n", i, bdir);
                            
                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--; pegouUmaFerramenta=true;
                        }
                        else if(b[bdir].laco>0 && (pegouUmaFerramenta && !pegouDuasFerramentas))
                        {
                            b[bdir].laco--;

                            printf(">> O robo %d pegou o laco da bancada %d\n", i, bdir);

                            sleep(4);
                            b[bdir].laco++;

                            printf("> O robo %d devolveu o laco para a bancada %d\n", i, bdir);

                            pthread_cond_broadcast(&cond_marteloMachadoLaco); ferrUsadas--; pegouDuasFerramentas=true;
                        }else if(b[bdir].machado>0 && (pegouUmaFerramenta && pegouDuasFerramentas))
                        {
                            b[bdir].machado--;

                            printf(">> O robo %d pegou o machado da bancada %d\n", i, bdir);

                            sleep(2);
                            b[bdir].machado++;

                            printf("> O robo %d devolveu o machado para a bancada %d\n", i, bdir);

                            pthread_cond_broadcast(&cond_machadoLaco); ferrUsadas--;
                        }
                    }
                }

                break;
            default:
                printf("mlk como conseguiste pegar um tipo fora de 7 pela funcao rand\n");
                printf("se ele entrar aqui em algum momento, eh porque a funcao rand falhou\n");
                break;
        }

        printf("[RAPAIZ!] O produto %d esta pronto para entrega\n", j);
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
    for(int i=0; i<PRODUTOS; i++) p[i].tipo = rand()%8;

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
    // updates: descobri pra que que o join serve
    for(int i=0; i<ROBOS; i++) pthread_join(robos[i], NULL);

    return 0;
}

