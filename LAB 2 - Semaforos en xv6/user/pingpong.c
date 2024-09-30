#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"


int main(int argc, char** argv){

    // en caso de pasar mas o menos argumentos 
    if(argc != 2){
        printf("ERROR: se necesita un solo argumento, un numero natural\n");
        return 0;
    }

    // transformamos el argumento pasado como string a un numero entero
    int N = atoi(argv[1]);

    // verificamos que sea mayor a cero
    if (N < 1){
        printf("ERROR: debe ser un numero mayor a cero\n");
        return 0;
    }

    // inicializamos los semaforos
    int padre = sem_open(0, 1);
    int hijo = sem_open(1, 0);

    int id = fork();

    if (id == 0){
        // HIJO
        for (unsigned int i = 0; i < N; i++){
            sem_down(hijo); 
            printf("\t pong \n");
            sem_up(padre);
        }
    }
    else{
        // PADRE
        for (unsigned int i = 0; i < N; i++){
            sem_down(padre); 
            printf("ping \n");
            sem_up(hijo);
        }
    }
    
    wait(0);

    // cerramos los semaforos
    sem_close(hijo);
    sem_close(padre);

    // retorno exitoso
    return 0;
}
