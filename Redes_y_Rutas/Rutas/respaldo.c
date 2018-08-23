#include "scheduler.h"
#include "lcgrand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//valores acorde a la carga de trafico que se asigna en la tarea
//#define LAMBDA 0.0033
//#define MU 0.001
#define LLEGADAS 1000000
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
		index[i]=route[i]/2; // se almacenan los indices de enlace	
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

void CalcularProbBlockUsr(double bloqueos[],double llegadas[],int usuarios,int nodos, FILE *archivo){
	int origen, destino;
	double cuenta=0;
	long double prob_usr;
	for (origen = 0; origen <= nodos; origen++){	
		for (int destino = 0; destino <= nodos; destino++){
			cuenta=cuenta+llegadas[(origen*nodos)+destino];
			if (origen==destino){
				fprintf(archivo, "%i\t%i\t%f\t%f\tNO TIENE\n",origen,destino, llegadas[(origen*nodos)+destino],bloqueos[(origen*nodos)+destino]);
			}
			else if (llegadas[(origen*nodos)+destino]==0)
			{
				fprintf(archivo, "%i\t%i\t%f\t%f\tNO REALIZO ENVIOS\n",origen,destino, llegadas[(origen*nodos)+destino],bloqueos[(origen*nodos)+destino]);
			}
			else{
				prob_usr = bloqueos[(origen*nodos)+destino]/llegadas[(origen*nodos)+destino];
				fprintf(archivo, "%i\t%i\t%f\t%f\t%LF\n",origen,destino, llegadas[(origen*nodos)+destino],bloqueos[(origen*nodos)+destino],prob_usr);
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


 	/*fprintf(archivo1,"%s:",argv[text]);
	fprintf(archivo2, "********************************************************\n");	
	fprintf(archivo2, "RED:%s\n",argv[text] );
	fprintf(archivo2, "********************************************************\n");*/

	
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
		//imprimir();
		//fclose(archivo);
}

static Evento *pScheduler=NULL;

int main(int argc, char const *argv[]){

	FILE *archivo;
	FILE *PBR;
	FILE *PBU;

	PBR = fopen("Prob_Block_RED.txt","w");
	PBU = fopen("Prob_Block_USR.txt","w");
 	
 	//char caracteres[100];
 	//char *pointer;

 	//Evento *pScheduler;

 	long double Toff = (Ton-(Ton*Q))/Q;
 	long double MU = 1.0/Ton;
 	long double LAMBDA = 1.0/(Ton+Toff);
 	//int NODOS;
 	//int ENLACES;
 	int t_source;
 	int t_dest;
 	int t_hop;
 	long double p_block; // Probabilidad de bloqueo

	for(int text = 1; text < argc; text++){ 
		contador_global=1;
		p_block = 1;
		int canales=1;//cantidad de canales por enlace (parte en 1)
		fprintf(PBR,"%s:",argv[text]);
		fprintf(PBU, "********************************************************\n");	
		fprintf(PBU, "RED:%s\n",argv[text] );
		fprintf(PBU, "********************************************************\n");	
	 	//for(canales;canales<=30;canales++){
		while(p_block!=0){
			Init(argv[text],LAMBDA);
	 		/*archivo = fopen(argv[text],"r");

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
			//imprimir();
			fclose(archivo);*/

			if (canales==1){
				fprintf(PBR,"%i:",NODOS);
				fprintf(PBR,"%i:",ENLACES);
			}



			int USR = NODOS*NODOS;
			int CAPACIDAD[ENLACES/2]; //Capacidad de todos los enlaces
			double BLOCK_X_USR[USR];//Array con la cantidad de bloqueos de cada usuario
			double ENV_X_USR[USR]; //Array con la cantidad de envios que realiza cada usuario
			long double bloqueo = 0; // Bloqueos totales de la red

			printf("%i\n",ENLACES);
			printf("%i\n",NODOS );
			
			
			LlenarEnlaces(canales,ENLACES/2,CAPACIDAD);
			llenarConCeros(USR,BLOCK_X_USR);
			llenarConCeros(USR,ENV_X_USR);
			
			for (int y = 0; y < LLEGADAS; y++){ // Iterar hasta completar "LLEGADAS" (10.000.000 de veces)
				pScheduler = pop();
				int t_link[pScheduler->saltos];//array temporal para guardar id's de enlace a analizar
				ObtenerIndices(pScheduler->saltos,pScheduler->ruta,t_link);
				int libre = IsFree(0,pScheduler->saltos,CAPACIDAD,t_link); // variable que indica si la ruta esta libre para transmitir

				if (pScheduler->tipo == 0){// Evento de llegada
					if (pScheduler->saltos==libre){
						for (int i = 0; i < pScheduler->saltos; i++){
							CAPACIDAD[t_link[i]]= CAPACIDAD[t_link[i]]-1; // se ocupan los canales respectivos de la ruta
						}
						push(crearEvento(1,pScheduler->tiempo+RandomTime(pScheduler->origen+NODOS,MU),pScheduler->origen,pScheduler->destino,pScheduler->saltos,pScheduler->ruta)); // se genera evento de salida
						ENV_X_USR[(pScheduler->origen*NODOS)+pScheduler->destino]++;
						
					}
					else{
						bloqueo++;// Se produce un bloqueo y se realiza retransmision
						push(crearEvento(0,pScheduler->tiempo+RandomTime(pScheduler->origen,LAMBDA),pScheduler->origen,pScheduler->destino,pScheduler->saltos,pScheduler->ruta)); // se retransmite evento de llegada
						BLOCK_X_USR[pScheduler->origen*NODOS+pScheduler->destino]++;
						ENV_X_USR[(pScheduler->origen*NODOS)+pScheduler->destino]++;
					}
				}
				else{// Evento de salida
					for (int i = 0; i < pScheduler->saltos; i++){
						if (CAPACIDAD[t_link[i]]>0)
						{
							CAPACIDAD[t_link[i]]= CAPACIDAD[t_link[i]]+1; // se liberan los canales de la ruta
						}
						else
							break;	
					}
					push(crearEvento(1,pScheduler->tiempo+RandomTime(pScheduler->origen+NODOS,MU),pScheduler->origen,pScheduler->destino,pScheduler->saltos,pScheduler->ruta));

				}
			
				free(pScheduler);
				
			}
			//printf("%LF\n",bloqueo );
			p_block = bloqueo/LLEGADAS;
			fprintf(PBR,"%LF:", p_block);
			fprintf(PBU, "********************************************************\n");
			fprintf(PBU, "Probablidad de Bloqueo por usuario con %i Long. de Onda:\n",canales );
			fprintf(PBU, "********************************************************\n");
			fprintf(PBU, "Origen\tDestino\tEnvios\tBloqueos\tProb.Bloqueo\n");
			fprintf(PBU, "********************************************************\n");

			CalcularProbBlockUsr(BLOCK_X_USR,ENV_X_USR, USR,NODOS,PBU);
			canales++;
			bloqueo=0;
		}
		fprintf(PBR, "\n" );
	}
	fclose(PBR);
	fclose(PBU);
	return 0;
}