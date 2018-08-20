#include "scheduler.h"
#include "lcgrand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//valores acorde a la carga de trafico que se asigna en la tarea
#define LLEGADAS 10000000
#define Q 0.3
#define Ton 0.001

int contador_global = 0;
int seed_ini = 0;
int ENLACES;
int NODOS;

int RandomSeed(){
	int semilla = rand()%50;
	return semilla;
}

float RandomTime(int seed,float lambda){
	float t;
	if (contador_global==99999){
		lcgrandst(2147483646,seed);
	 	t = -1*logf(1-lcgrand(seed))/lambda;
	 	contador_global=0;
	}
	else{
		t = -1*logf(1-lcgrand(seed))/lambda;
 		contador_global++;
	} 
	
	return (float)t;
}

void llenarConCeros(int usr,double array[]){
	for (int i = 0; i < usr; i++){
		array[i]=0;
	}
}

void LlenarEnlaces(int channel,int link,int array[]){//llena el arreglo de enlaces con una cantidad de canales
	for (int i = 0; i < link; i++) {//llenar todos los enlaces con "canales" 
		array[i]=channel;
	}
}

void ObtenerIndices(int hop,int route[],int index[]){
	for (int i = 0; i < hop; i++){
		index[i]=route[i]; // se almacenan los indices de enlace	
	}
}

int IsFree(int x,int hop,int array1[],int array2[]){
	for (int i = 0; i < hop; i++){//verificar si la ruta esta disponible	

		if (array1[array2[i]] > 0){
			x++; // el enlace se encuentra libre
		}
		else{
			x--; // el enlace se encuentra lleno
		}
	}
	return x;
}

void CalcularProbBlockUsr(double saltos[],double bloqueos[],double llegadas[],int usuarios,int nodos, FILE *archivo){
	int origen, destino;
	float cuenta=0;
	float prob_usr;
	for (origen = 0; origen < nodos; origen++){	
		for (int destino = 0; destino < nodos; destino++){
			cuenta=cuenta+llegadas[(origen*nodos)+destino];
			if (origen==destino){
				fprintf(archivo, "%i\t%i\tNO TIENE\n",origen,destino);
			}
			else if (llegadas[(origen*nodos)+destino]==0){
				fprintf(archivo, "%i\t%i\tNO REALIZO ENVIOS\n",origen,destino);
			}
			else{
				prob_usr = bloqueos[(origen*nodos)+destino]/llegadas[(origen*nodos)+destino];
				fprintf(archivo, "%i\t%i\t%f\t%f\n",origen,destino, prob_usr,saltos[(origen*nodos)+destino]);
			}		
		}
	}
}

void Init(const char *name,long double LAMBDA){

	FILE *archivo;

	char caracteres[100];
 	char *pointer;

 	int t_source;
 	int t_dest;
 	int t_hop;
	
	archivo = fopen(name,"r");

	if (archivo == NULL)
	 	exit(1);
	else{
	 	while (feof(archivo) == 0){
	 		fgets(caracteres,100,archivo);
	 		pointer=strtok(caracteres,"\t");
	 		if (strcmp(pointer,"Number")==0){
	 			while((pointer = strtok(NULL,"\t"))!=NULL){
	 				if (strcmp(pointer,"nodes:")==0){// valor nodo
	 					pointer=strtok(NULL,"\t");
	 					NODOS = atoi(pointer);
	 				}
	 				else if(strcmp(pointer,"links:")==0){ // valor enlaces
	 					pointer=strtok(NULL,"\t");
						ENLACES=atoi(pointer);
					}
				}
			}
				else if(strcmp(pointer,"Rutas")==0){//valor rutas, no sirve
					continue;
			}
			else if (strlen(pointer)>40){//============
				continue;
				}
			else if (strcmp(pointer,"source")==0){// source dest. hop.......
				continue;
			}
			else{ // valores importantes de conexion
				t_source = atoi(pointer);
				pointer = strtok(NULL,"\t");
				t_dest = atoi(pointer);
				if (t_source==t_dest){//ruta asi mismo
					continue;
				}
				pointer = strtok(NULL,"\t");
				t_hop = atoi(pointer);

				int t_path[t_hop-1];
					
				for (int i = 0; i < t_hop; i++){
					pointer = strtok(NULL,"\t");
					t_path[i]=atoi(pointer);
		 		}
		 		//Inicializar de scheduler, solo con llegadas
				push(crearEvento(0,RandomTime(t_source,LAMBDA),t_source,t_dest,t_hop,t_path));
		 	}
		}
	}
	fclose(archivo);
}

int Determinar_Canales(const char *texto){// canales necesarios por red, para obtener prob. de bloqueo cero

	int channel=0;

	if (strcmp(texto,"ArpaNet.rut")==0){
		return channel=23;
	}

	else if (strcmp(texto,"NSFNet.rut")==0){
		return channel=12;
	}

	else if (strcmp(texto,"EON.rut")==0){
		return channel=15;
	}

	else if (strcmp(texto,"EuroCore.rut")==0){
		return channel=4;
	}

	else if (strcmp(texto,"EuroLarge.rut")==0){
		return channel=47;
	}

	else if (strcmp(texto,"UKNet.rut")==0){
		return channel=17;
	}
}

int main(int argc, char const *argv[]){

	FILE *archivo;
	FILE *PBR;
	FILE *PBU;
	FILE *PB2;

	PBR = fopen("Prob_Block_RED.txt","w");
	PBU = fopen("Prob_Block_USR.txt","w");
	PB2 = fopen("Grafica2.txt","w");

 	Evento *pt;

 	double Toff = (Ton-(Ton*Q))/Q;
 	double MU = 1.0/Ton;
 	double LAMBDA = 1.0/(Ton+Toff);
 	double SIGMA = 1.0/(Toff);

 	int canales ;
 	float p_block; // Probabilidad de bloqueo

	for(int text = 1; text < argc; text++){ 
		contador_global=1;
		p_block = 0;
		canales = Determinar_Canales(argv[text]);
		int canales_temp=canales;
		fprintf(PBR,"%s:",argv[text]);
		printf("Iniciando Simulacion de %s...\n",argv[text]);
		fprintf(PBU, "+++\n");	
		fprintf(PBU, "RED:%s\n",argv[text] );
		fprintf(PBU, "********************************************************\n");	

		while(p_block<=0.001){
			if (schedulerIsEmpty()!=1){	
				for (int i = 0; i < NODOS*(NODOS-1); i++){
					pt = pop();
					free(pt);
				}
			}
			Init(argv[text],LAMBDA);

			if (canales==canales_temp){
				fprintf(PBR,"%i:",NODOS);
				fprintf(PBR,"%i:",ENLACES);
				fprintf(PBR,"%i:",canales);
			}

			int USR = NODOS*NODOS;
			int CAPACIDAD[ENLACES]; //Capacidad de todos los enlaces
			double BLOCK_X_USR[USR];//Array con la cantidad de bloqueos de cada usuario
			double ENV_X_USR[USR]; //Array con la cantidad de envios que realiza cada usuario
			double HOP_X_USR[USR];
			long double bloqueo = 0; // Bloqueos totales de la red
			
			LlenarEnlaces(canales,ENLACES,CAPACIDAD);
			llenarConCeros(USR,BLOCK_X_USR);
			llenarConCeros(USR,ENV_X_USR);
			llenarConCeros(USR,HOP_X_USR);
			int count=0;
			for (int y = 0; y < LLEGADAS; y++){ // Iterar hasta completar "LLEGADAS" (10.000.000 de veces)
				pt = pop();
				int t_link[pt->saltos];//array temporal para guardar id's de enlace a analizar
				ObtenerIndices(pt->saltos,pt->ruta,t_link);
				int libre = IsFree(0,pt->saltos,CAPACIDAD,t_link); // variable que indica si la ruta esta libre para transmitir
				
				if (pt->tipo == 0){// Evento de llegada
					if (pt->saltos==libre){
						for (int i = 0; i < pt->saltos; i++){
							CAPACIDAD[t_link[i]]= CAPACIDAD[t_link[i]]-1; // se ocupan los canales respectivos de la ruta
						}
						push(crearEvento(1,pt->tiempo+RandomTime(pt->origen+NODOS,MU),pt->origen,pt->destino,pt->saltos,pt->ruta)); // se genera evento de salida
						ENV_X_USR[(pt->origen*NODOS)+pt->destino]++;
						HOP_X_USR[(pt->origen*NODOS)+pt->destino]=pt->saltos;
						free(pt);
						
					}
					else{
						bloqueo++;// Se produce un bloqueo y se realiza retransmision
						push(crearEvento(0,pt->tiempo+RandomTime(pt->origen,LAMBDA),pt->origen,pt->destino,pt->saltos,pt->ruta)); // se retransmite evento de llegada
						BLOCK_X_USR[pt->origen*NODOS+pt->destino]++;
						ENV_X_USR[(pt->origen*NODOS)+pt->destino]++;
						HOP_X_USR[(pt->origen*NODOS)+pt->destino]=pt->saltos;
						free(pt);
					}
				}
				else{// Evento de salida
					for (int i = 0; i < pt->saltos; i++){
						if (CAPACIDAD[t_link[i]]<canales)
						{
							CAPACIDAD[t_link[i]]= CAPACIDAD[t_link[i]]+1; // se liberan los canales de la ruta
						}	
					}
					push(crearEvento(0,pt->tiempo+RandomTime(pt->origen+NODOS,SIGMA),pt->origen,pt->destino,pt->saltos,pt->ruta));
					free(pt);
				}			
				
			}
			p_block = bloqueo/LLEGADAS;
			fprintf(PBR,"%f:", p_block);

			if (p_block>=0.001){ //Texto informativo del resultado de simulacion
				fprintf(PBU, "********************************************************\n");
				fprintf(PBU, "Probablidad de Bloqueo por usuario con %i Long. de Onda:\n",canales);
				fprintf(PBU, "Probablidad de Bloqueo total de la Red: %f\n",p_block);
				fprintf(PBU, "********************************************************\n");
				fprintf(PBU, "Origen\tDestino\tProb.Bloqueo\tSaltos\n");
				fprintf(PBU, "********************************************************\n");
				CalcularProbBlockUsr(HOP_X_USR, BLOCK_X_USR,ENV_X_USR, USR,NODOS,PBU);
				fprintf(PBR,"%f:", p_block);
				







				/*fprintf(PB2, "%s:",argv[text]);// Texto necesario para la segunda grafica
				for (int i = 0; i < 10; i++){
					fprintf(PB2, "%f:", BLOCK_X_HOP[i]/ENV_X_HOP[i]);	
				}
				fprintf(PB2, "\n");*/		
			}
		
			canales--;
			bloqueo=0;
		}
		fprintf(PBR, "\n" );
		printf("Finalizando Simulacion de %s\n",argv[text]);
	}
	fclose(PBR);
	fclose(PBU);
	fclose(PB2);
	return 0;
}

