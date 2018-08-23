#include "scheduler.h"
#include <stdlib.h>

static Evento *pScheduler=NULL;

void pushEvento(Evento* pEvento){

  if(pScheduler==NULL){
    pScheduler=pEvento;
    return;
  }
  Evento *p=pScheduler;

  if(p->tiempo>pEvento->tiempo){
    pEvento->next=p;
    p=NULL;
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

Evento *crearEvento(int tipo,float tiempo,int source,int dest,int hops,int *path){
  Evento *p=malloc(sizeof(Evento));
  p->tipo=tipo;
  p->tiempo=tiempo;
  p->source=source;
  p->dest=dest;
  p->hops=hops;
  p->path=path;
  p->next=NULL;
  return p;
}

Evento *popEvento(){
  Evento *p=pScheduler;
  pScheduler=pScheduler->next;
  p->next=NULL;
  return p;
}

void imprimirScheduler(){
  Evento *p=pScheduler;
  int i;
  i = 0;
  while(p!=NULL){
    printf("(%i,%f,%i,%i,%i)+(",p->tipo,p->tiempo,p->source,p->dest,p->hops);
    for (i = 0; i < p->hops; ++i)
    {
      if (i == ((p->hops)-1)) printf("%i), ",*(p->path+i));
      else printf("%i, ",*(p->path+i));
    }
    p=p->next;
  }
  printf("\n");
  return;
}

int schedulerIsEmpty(){
  return pScheduler==NULL;
}

void freeScheduler(){
  Evento *p=pScheduler;

  while(p!=NULL){
    pScheduler = p->next;
    free(p);
    p=pScheduler;
  }
}