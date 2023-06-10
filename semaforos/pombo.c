#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CAPACIDADE 20 //quantidade de cartas na mochila

sem_t cheia;
sem_t enchendo;
sem_t mutex;
int cartas = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);



int main(int argc, char **argv){
    int i;

    sem_init(&cheia,0,0);
    sem_init(&enchendo,0,CAPACIDADE);
    sem_init(&mutex,0,1);

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
     pthread_t pombo;
     id = (int *) malloc(sizeof(int));
     *id = 0;
     pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

     pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
    int i = 0;
    while(1){
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        sem_wait(&cheia);
        //Leva as cartas para B e volta para A
        printf("\n\nPombo, mochila cheia vou levar as cartas: %d \n",cartas);
        sleep(5); 
        cartas = 0;
        printf("\n\nPombo, mochila vazia vou voltar: %d \n",cartas);
        sleep(3);
        printf("\n\nPombo, voltei! \n");
        
        //Acordar os usuários
        for(i=0; i < CAPACIDADE;i++){
            sem_post(&enchendo);
        }   
    }
}

void * f_usuario(void *arg){
    int id = *((int *) arg);
    while(1){
	//Escreve uma carta
        sleep(2);
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        sem_wait(&enchendo); 
        //Posta sua carta na mochila do pombo
        sem_wait(&mutex); 
        	cartas++;
        	printf("\nUsuario %d, coloquei a carta número %d.\n",id,cartas);
        	sleep(1);
        	//Caso a mochila fique cheia, acorda o ṕombo
        	if(cartas == CAPACIDADE){
            		sem_post(&cheia);
        	}
        sem_post(&mutex); 
    }
}
