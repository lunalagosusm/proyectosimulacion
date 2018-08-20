#ifndef SCHEDULER_H
#include <stdio.h>

typedef struct mnodo{
  int tipo;
  float tiempo;
  int origen;
  int destino;
  int saltos;
  int ruta[8];
  struct mnodo *next;
}Evento;

//static Evento *pScheduler=NULL;
void push(	Evento* pEvento);
Evento *crearEvento(int tipo,float tiempo,int origen, int destino, int saltos,int *ruta);
Evento *pop();

void imprimir();
int schedulerIsEmpty();
#endif