#ifndef SCHEDULER_H
#include <stdio.h>

typedef struct mnodo{
  int tipo;
  float tiempo;
  int source;
  int dest;
  int hops;
  int *path;
  struct mnodo *next;
}Evento;


void pushEvento(Evento* pEvento);
Evento *crearEvento(int tipo,float tiempo,int source,int dest,int hops,int *path);
Evento *popEvento();

void imprimirScheduler();
void freeScheduler();
int schedulerIsEmpty();
#endif