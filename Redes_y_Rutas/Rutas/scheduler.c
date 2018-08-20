#include "scheduler.h"
#include <stdlib.h>

static Evento *pScheduler=NULL;

void push(Evento* pEvento){
  if(pScheduler==NULL){
    pScheduler=pEvento;
    return;
  }

  Evento *p=pScheduler;
  if(p->tiempo>pEvento->tiempo){
    pEvento->next=p;
    pScheduler= pEvento;
    return;
  }

  while(p->next!=NULL){
    if(p->next->tiempo>pEvento->tiempo){
      pEvento->next=p->next;
      p->next=pEvento;
      return;
    }
    p=p->next;
  }

  p->next=pEvento;
  return;
}

Evento *crearEvento(int tipo,float tiempo,int origen, int destino, int saltos,int ruta[]){
  Evento *p=malloc(sizeof(Evento));
  p->tipo=tipo;
  p->tiempo=tiempo;
  p->origen = origen;
  p->destino = destino;
  p->saltos = saltos;
  for (int i = 0; i < p->saltos; i++){
    p->ruta[i]=ruta[i];  
  }
  p->next=NULL;
  return p;
}

Evento *pop(){
  Evento *p=pScheduler;
  pScheduler=pScheduler->next;
  p->next=NULL;
  return p;
}

void imprimir(){
  Evento *p=pScheduler;
  while(p!=NULL){
    printf("[TIPO:%i,TIEMPO:%f,origen:%i,destino:%i,saltos:%i]\n",p->tipo,p->tiempo,p->origen,p->destino,p->saltos);
    printf("Ruta:[");
    for (int i = 0; i < p->saltos;i++){
      printf("%i ",*(p->ruta+i));
    }
    printf("]\n");
    p=p->next;
  }
  printf("\n");
  return;
}

int schedulerIsEmpty(){
  return pScheduler==NULL;
}