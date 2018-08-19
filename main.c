#include <stdlib.h>
#include "lcgrand.h"
#include "scheduler.h"
#include "Topologias.h"
#include "ospf.h"
#include <stdio.h>
#include <math.h>

int blocked; //Usuarios bloqueados
int llegadasExe; //Llegadas de usuarios ejecutadas
int llegadasTot;// Llegadas Ejecutadas + Bloqueados
double MU;//Tasa media de servicio
double LAMBDA;//Tasa media de llegada
double LAMBDAPRIMA;
double ton;//tiempo medio enviando
double toff;//tiempo medio no enviando
double carga;
int numEnlaces;//Numero de enlaces de la red
int USUARIOS;//Numero de usuarios de la red: NODOS*(NODOS-1)
int NODOS;//Numero de nodos de la res
int LINKS;//Numero de enlaces en la red
int MAX_HOPS;//Maximo largo de ruta
int COUNTLCG[101];
int CAPACIDAD;//Capacidad de cada enlace
float * cnxBlocked;//Bloqueos por usuario
float * cnxLlegadas;//llegadas por usuario
float * cnxEject;//llegadas ejecutadas por usuario
int * canalesLibres;//Para llevar cuenta de los canales libres
int * seed;//Arreglo de Semillas lcgrand
int datosRed[3];//Datos generales de la red (nro. cnx, nro. nodos y nro. links)
int **datosRutas;//Matriz con todas los datos de cada conexion; exepto el path
int **datosPath;//Matriz con el path de cada conexion
float prob_Bloq;
float *Prob_user;//promedio de probabilidades por largo de ruta
int enlacesCritico[3];

void dataRed(FILE *fp, int datosRed[3]);//Obtiene datos generales de la red
int **getDataRoute(FILE *fp, int nodos);//Obtiene datos de especificos de la ruta
int **getPathRoute(FILE *fp, int N, int ** datosRutas);//Obtiene path de la ruta
void readNetwork(int numIteracion);//Manejo de Textos
void Ini();//Se fijan las condiciones iniciales
void Arribo(Evento* p);//Se procesan los arribos(llegadas)
void Salida(Evento* p);//Se procesan las salidas
void generaAS(int tipo, float tasaMedia, float tiempo_actual, int source,
              int dest, int hops, int * path);//Genera evento Arribo o Salida
float calculaTiempo(int tipo,float tasaMedia);//Calcula tiempo que ayudara a obtener tiempos futuros
int isFreeRoute(Evento* p);//Evalua si esta libre la ruta
void sumaCanal(Evento *p);//Suma 1 a canalesLibres
void restaCanal(Evento *p);//Resta 1 a canalesLibres
void enlacesCriticos();
void maxHops();

int main(){
    Evento *p; //puntero al scheduler
    int i,j,k,w,l;
    p = NULL;
    prob_Bloq = 0.0;
    carga = 0.3;
    ton = 0.001;
    toff = (ton/carga)-ton;
    MU = (1.0/ton);
    LAMBDA = 1.0/(ton+toff);
    LAMBDAPRIMA = 1.0/(toff);
    int enlacesCritico[3];
    for(i=0; i < 1; ++i)// Ejecucion 6 simulaciones (una por red)
    {
        readNetwork(i);

        while(prob_Bloq<=0.001)//Criterio de parada Simulador
        {
            Ini();
            printf("%i;", canalesLibres[0]);
            while(llegadasTot<pow(10,5))
            {
                p = popEvento();//Extrae evento
                if(p->tipo < USUARIOS) Arribo(p);//Verifica si es arribo o salida
                else Salida(p);
                free(p);//Libera memoria
            }

            prob_Bloq = ((float)blocked/(float)llegadasTot);
            printf("%i;", blocked);
            --CAPACIDAD;

            freeScheduler();//Liberar Scheduler para nueva iteración
        }
        /*___Se calcula el promedio de las probabilidades por usuario y largo de red*/
        printf("probuser;");
        for (l = 1; l <= MAX_HOPS; ++l)
        {
            k=0;
            w=0;
            for(j = 0; j<NODOS*NODOS;++j)
            {
                if(datosRutas[j][0]==datosRutas[j][1])
                {
                    w=w;
                }
                else
                {
                    if(datosRutas[j][2] == l)
                    {
                        Prob_user[l-1]=(Prob_user[l-1])+(cnxBlocked[w]/cnxLlegadas[w]);
                        ++k;
                    }
                    else
                    {
                        Prob_user[l-1]=(Prob_user[l-1]);
                        k=k;
                    }
                    ++w;
                }
            }
            Prob_user[l-1] =(float) (Prob_user[l-1]/k);
            printf("%f;",Prob_user[l-1]);
        }
        /*__ Se reinician variables y se libera la memoria para proxima iteración___*/
        prob_Bloq = 0.0;
        for (k = 0; k < NODOS*NODOS; ++k){
            free(datosPath[k]);
            free(datosRutas[k]);
        }
        free(datosPath);
        free(datosRutas);
        free(seed);
        free(canalesLibres);
        free(cnxBlocked);
        free(cnxEject);
        free(cnxLlegadas);
        free(Prob_user);
    }
}
void maxHops(){
    int i,hops;
    for (i = 0; i < NODOS*NODOS; ++i)
    {
        hops = datosRutas[i][2];

        if (MAX_HOPS < hops)MAX_HOPS = hops;
        else MAX_HOPS = MAX_HOPS;
    }
}
void enlacesCriticos(){
    int cuentaEnlaces[LINKS/2];
    int max[3];
    int i, j;
    int source, dest, hops;
    //int enlacesCritico[3];
    enlacesCritico[0]=1000;
    enlacesCritico[1]=1000;
    enlacesCritico[2] = 1000;
    max[0] = 0;
    max[1] = 0;
    max[2] = 0;
    for (i = 0; i < LINKS/2; ++i)
    {
        cuentaEnlaces[i]=0;
    }
    for (i = 0; i < NODOS*NODOS; ++i)
    {
        source = datosRutas[i][0];
        dest = datosRutas[i][1];
        hops = datosRutas[i][2];
        if (source != dest)
        {
            for (j = 0; j < hops; ++j)
            {
                //printf("datosPath: %i\t",datosPath[i][j] );
                cuentaEnlaces[datosPath[i][j]] += 1;
            }
        }
        else{
            continue;
        }
    }
    printf("lista cuenta enlaces:\n");
    for (i = 0; i < LINKS/2; ++i)
    {
        printf("%i ",cuentaEnlaces[i]);
    }
    printf("numero enlaces : %i\n", i);
    printf("PRUEBA0\n");
    for (j = 0; j < LINKS/2; ++j)
    {
        if(max[0] < cuentaEnlaces[j]){
            max[0] = cuentaEnlaces[j];
            enlacesCritico[0] = j;
        }
        else{
            max[0] = max[0];
        }
    }
    printf("PRUEBA1\n");
    for (j = 0; j < LINKS/2; ++j)
    {
        if( (max[1] < cuentaEnlaces[j]) && (j != enlacesCritico[0]) ){
            max[1] = cuentaEnlaces[j];
            enlacesCritico[1] = j;
        }
        else{
            max[1] = max[1];
        }
    }
    printf("PRUEBA2\n");
    for (j = 0; j < LINKS/2; ++j)
    {
        //printf("j:%i,",j );
        if( (max[2] < cuentaEnlaces[j]) && ( (j != enlacesCritico[0]) && (j != enlacesCritico[1]) ) ){
            //printf("max2: %i\n", max[2]);
            max[2] = cuentaEnlaces[j];
            enlacesCritico[2] = j;
        }
        else{
            max[2] = max[2];
            enlacesCritico[2] = enlacesCritico[2];
        }
    }
    printf("PRUEBA3\n");
    printf("\nenlaces Criterios: %i, %i, %i; ", enlacesCritico[0],enlacesCritico[1],enlacesCritico[2]);
    printf("PRUEBA4\n");
}
void dataRed(FILE *fp, int datosRed[3]){
    char linea[100];
    char num[3];

    int i, j, k;
    for(i = 0; i<3; i++){
        fgets(linea, 100,fp);
        for(j=0;j<100; j++){
            if( linea[j] >= 48 && linea[j] <= 57){
                num[0] = linea[j];
                num[1] = linea[j+1];
                num[2] = linea[j+2];
                j = j+2;
                datosRed[i] =atoi(num);
                break;
            }
        }
    }
}
int **getDataRoute(FILE *fp, int nodos){
    char linea[100];
    char num[3];
    int i, j,k;
    int **mat;
    int cnx;
    rewind(fp);
    for (i = 0; i < 6; ++i)
    {
        fgets(linea, 100,fp);
    }
    cnx = nodos*nodos;
    mat = (int **) malloc(cnx*sizeof(int *));
    for (i = 0; i < cnx; ++i)
    {
         mat[i] = (int *)malloc(3*sizeof(int));
    }
    j = 0;
    k = 0;
    while( feof(fp) == 0 ){
        fgets(linea, 100,fp);

        for(i=0;i<100; i++){
            if((linea[i]>=48)&&(linea[i]<=57)){
                num[0] = linea[i];
                //++i;
                if ((linea[i]>=48)&&(linea[i]<=57))
                {
                    num[1] = linea[i+1];
                    ++i;
                    if ((linea[i]>=48)&&(linea[i]<=57))
                    {
                        num[2] = linea[i+1];
                        ++i;
                    }
                }
            }
            if(k == 2) {
                mat[j][k] = atoi(num);
                ++j;
                k=0;
                break;
            }
            else{
                mat[j][k] = atoi(num);
                ++k;
            }
        }
    }
    return mat;
}
int **getPathRoute(FILE *fp, int N, int ** datosRutas){
    char linea[100];
    char num[4];
    int i,j,k;
    rewind(fp);
    int ** mat;
    int f,c;
    for (i = 0; i < 6; ++i)
    {
        fgets(linea, 100,fp);
    }

    f = N*N;//numero de conexiones o usuarios
    mat = (int **) malloc(f*sizeof(int*));

    for (i = 0; i < f; ++i)
    {
        c = datosRutas[i][2];
        if(c == 0){
            c = 1;
            mat[i] = (int *)malloc(c*sizeof(int));
        }
        else mat[i] = (int *)malloc(c*sizeof(int));
    }

    j=0;
    k=0;
    int countab;
    countab = 0;
    num[0]=10;num[1]=10;num[2]=10;
    while( feof(fp) == 0 ){
        fgets(linea, 100,fp);
        for(i=0;i<100; i++){
            if (countab < 3)
            {
                if (linea[i] == 9)
                {
                    ++countab;
                }
                else countab = countab;
            }
            else
            {
                if(linea[i] == 10){
                    break;
                }
                else{
                    if((linea[i]>=48)&&(linea[i]<=57)){
                        num[0] = linea[i];
                        if((linea[i+1]>=48)&&(linea[i+1]<=57))
                        {
                            ++i;
                            num[1] = linea[i];
                            if ((linea[i+1]>=48)&&(linea[i+1]<=57))
                            {
                                ++i;
                                num[2] = linea[i];
                                num[3] = 10;
                            }
                            else{num[2]=10;num[3] = 10;}
                        }
                        else{num[1]=10;num[2]=10;num[3] = 10;}

                        if (datosRutas[j][2] == 0)
                        {
                            num[0]=10;num[1]=10;num[2]=10;num[3] = 10;
                            mat[j][k] = 0;
                        }
                        else{
                            mat[j][k] = atoi(num);
                        }
                        ++k;
                    }
                    else{num[0]=10;num[1]=10;num[2]=10;num[3] = 10;}
                }
            }
        }
        ++j;
        countab=0;
        k=0;
    }
    return mat;
}
void readNetwork(int numIteracion){
    //________MANEJO DE TEXTOS________________
    FILE *fp;

    switch(numIteracion)//1 por cada topologia)(0 a 5)
    {
        case 0:
            /*fp = fopen("EuroCore.rut", "r");
            CAPACIDAD = 8;
            printf("EuroCore.rut;");*/
            generaRuta("/Users/pedro/Desktop/proyecto/ArpaNet.top","/Users/pedro/Desktop/proyecto/ArpaNet.rut");
            fp = fopen("/Users/pedro/Desktop/proyecto/ArpaNet.rut","r");
            CAPACIDAD = 8;
            break;
        default://case 1:
            /*fp = fopen("EON.rut", "r");
            CAPACIDAD = 20;
            printf("EON.rut;");*/
            generaRuta("/Users/pedro/Desktop/proyecto/ArpaNet.top","/Users/pedro/Desktop/proyecto/ArpaNet.rut");
            fp = fopen("/Users/pedro/Desktop/proyecto/ArpaNet.rut","r");
            CAPACIDAD = 20;
            break;
        //case 2:
           /* fp = fopen("ArpaNet.rut", "r");
            CAPACIDAD = 25;
            printf("ArpaNet.rut;");
            break;
        case 3:
            fp = fopen("EuroLarge.rut", "r");
            CAPACIDAD = 47;
            printf("EuroLarge.rut;");
            break;
        case 4:
            fp = fopen("NSFNet.rut", "r");
            CAPACIDAD = 15;
            printf("NSFNet.rut;");
            break;
        default:
            fp = fopen("UKNet.rut", "r");
            CAPACIDAD = 21;
            printf("UKNet.rut;");
            break;*/
    }

    dataRed(fp, datosRed);

    NODOS = datosRed[1];
    USUARIOS=NODOS*(NODOS-1);
    LINKS = datosRed[2];

    datosRutas = getDataRoute(fp, NODOS);
    datosPath = getPathRoute(fp, NODOS, datosRutas);

    maxHops();
    enlacesCriticos();
    printf("\nenlaces Criterios: %i, %i, %i; ", enlacesCritico[0],enlacesCritico[1],enlacesCritico[2]);
    crear_top(enlacesCritico[0],"ArpaNet_new.top","ArpaNet1.top");
    crear_top(enlacesCritico[1],"ArpaNet_new.top","ArpaNet2.top");
    crear_top(enlacesCritico[2],"ArpaNet_new.top","ArpaNet3.top");
    fclose(fp);
    //_____________________________________________
}
void Ini()//No existen salidas si no hay llegadas
{
    int i,j;

    blocked = 0;
    llegadasExe = 0;
    llegadasTot = 0;
    MAX_HOPS = 0;
    for (i = 0; i < 101; ++i)
    {
        COUNTLCG[i] = 0;
    }
    cnxBlocked = malloc(USUARIOS*sizeof(float));
    cnxLlegadas = malloc(USUARIOS*sizeof(float));
    cnxEject = malloc(USUARIOS*sizeof(float));
    for (i = 0; i < USUARIOS; ++i)
    {
        cnxBlocked[i] = 0;
        cnxLlegadas[i] = 0;
        cnxEject[i] = 0;
    }
    //___INICIALIZACION DE CANALES LIBRES_____
    canalesLibres = malloc(LINKS*sizeof(int));//Arreglo que maneja los canales libres

    for (i = 0; i < LINKS; ++i)
    {
        canalesLibres[i] = CAPACIDAD;
    }

    int source, dest, hops;
    //___INICIALIZACION DE SEMILLAS____________________
    seed = malloc((USUARIOS*2)*sizeof(int));
    j=0;
    for (i = 0; i < USUARIOS*2; ++i)
    {
        if(j==100) j=0;
        else ++j;
        seed[i]=j;
    }
    //___INICIALIZACION DE USUARIOS INICIALIES_________
    j=0;
    for (i = 0; i < NODOS*NODOS; ++i)
    {
        source = datosRutas[i][0];
        dest = datosRutas[i][1];
        hops = datosRutas[i][2];
        if (source == dest)
        {
            j=j;
        }
        else{
            generaAS(j, LAMBDA, 0.0, source, dest, hops, datosPath[i]);//Genera arribos
            ++j;
        }
    }
    Prob_user  = malloc(MAX_HOPS*sizeof(float));
    for (i = 0; i < MAX_HOPS; ++i)
    {
            Prob_user[i] = 0;
    }
}
float calculaTiempo(int tipo, float tasaMedia) //Calcula un tiempo que sera usado para obtener tiempos futuros
{
    float time, unif;
    unif = (float) lcgrand(seed[tipo]);//Numero uniforme entre [0,1]
    time = (-1)*(log(1-unif))/tasaMedia;

    return time;
}
void generaAS(int tipo, float tasaMedia, float tiempo_actual, int source,
              int dest, int hops, int * path)
{
    Evento *p;
    float tiempo;

    p = NULL;
    int i;

    tiempo = calculaTiempo(tipo,tasaMedia) + tiempo_actual;
    p = crearEvento(tipo, tiempo, source, dest, hops, path);

    pushEvento(p);
}
int isFreeRoute(Evento *p){
    int i, valor;
    for (i = 0; i < p->hops; ++i)
    {
        if (canalesLibres[(*(p->path+i))] == 0){
            return 0;
        }
        else valor = 1;
        return valor;
    }
    return 0;
}
void sumaCanal(Evento *p){
    int i;
    for (i = 0; i < p->hops; ++i)
    {	if(canalesLibres[(*(p->path+i))] == CAPACIDAD)
            canalesLibres[(*(p->path+i))] = CAPACIDAD;
        else canalesLibres[(*(p->path+i))]+=1;
    }
}
void restaCanal(Evento *p){
    int i;
    for (i = 0; i < p->hops; ++i)
    {	if(canalesLibres[(*(p->path+i))] == 0) canalesLibres[(*(p->path+i))]=0;
        else canalesLibres[(*(p->path+i))]-=1;
    }
}
void Arribo(Evento *p)
{
    Evento *eventoArribo;
    float tiempo_actual;
    float prob_Bloq;

    eventoArribo = p;
    tiempo_actual = eventoArribo->tiempo;

    if(isFreeRoute(eventoArribo) == 1)// ¿Hay espacio en el servidor?, se genera salida
    {
        restaCanal(eventoArribo); //se ocupa un espacio el servidor
        llegadasExe += 1;
        cnxEject[eventoArribo->tipo] +=1 ;
        generaAS((eventoArribo->tipo)+USUARIOS, MU, tiempo_actual, eventoArribo->source,eventoArribo->dest,eventoArribo->hops,eventoArribo->path);//Genera salida
    }
    else//No hay espacio en el servidor, se genera otra llegada
    {
        blocked+=1;
        cnxBlocked[eventoArribo->tipo]+=1;
        generaAS(eventoArribo->tipo, LAMBDAPRIMA, tiempo_actual,eventoArribo->source,eventoArribo->dest,eventoArribo->hops,eventoArribo->path);//Genera arribo
    }

    llegadasTot = llegadasExe + blocked;
    cnxLlegadas[eventoArribo->tipo] = cnxEject[eventoArribo->tipo] + cnxBlocked[eventoArribo->tipo];
}
void Salida(Evento* p)
{
    Evento *eventoSalida;
    float tiempo_actual;

    eventoSalida = p;
    tiempo_actual = eventoSalida->tiempo;
    generaAS((eventoSalida->tipo)-USUARIOS, LAMBDAPRIMA, tiempo_actual,eventoSalida->source,eventoSalida->dest,eventoSalida->hops,eventoSalida->path);//Genera Arribo
    sumaCanal(eventoSalida); //Se desocupa un espacio en el servidor
}
