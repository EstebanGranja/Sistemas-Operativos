#include <stdbool.h>
#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

#define MAX_SEM 128

struct semaphore{
    int value;
    int used;              // valor inicial del semaforo
    struct spinlock lock;   // sincronizacion entre procesos
};

struct semaphore semaphores[MAX_SEM];
// semaphores[sem] = sem.value  --->  asigna el valor inicial value al semaforo almacenado en la posicion sem

// abre y/o inicializa el semaforo 'sem' con el valor arbitrario 'value'
int 
sem_open(int sem, int value){
    
    // Devuelve 0 en caso de 'sem' invalido
    if(sem < 0 || sem >= MAX_SEM){
        return 0;
    }
    int new_sem=0;
    while (semaphores[new_sem].used)
    {
        new_sem++;
    }
    // toma el lock una vez este disponible, bloquea otros procesos que intentan acceder a el
    acquire(&semaphores[new_sem].lock);
    semaphores[new_sem].used=1;
    semaphores[new_sem].value = value;

    // libera el lock
    release(&semaphores[new_sem].lock);
    return new_sem;
}

// libera la memoria del semaforo 'sem'
int sem_close(int sem){
    // Devuelve 0 en caso de 'sem' invalido
    if(sem < 0 || sem >= MAX_SEM){
        return 0;
    }

    // toma el lock una vez este disponible, bloquea otros procesos que intentan acceder a el
    acquire(&semaphores[sem].lock);

    semaphores[sem].used = 0;

    // libera el lock
    release(&semaphores[sem].lock);

    return sem;
}

// decrementa el valor del semaforo 'sem', bloquea los procesos si su valor es cero
int sem_down(int sem){
    //si el semaforo no esta inicializado, devuelve error
    if(sem < 0 || sem >= MAX_SEM || semaphores[sem].value < 0) {
        printf("ERROR: semaforo no valido o no inicializado\n");
        return 0;
    }

    //toma el lock una vez este disponible, bloquea otros procesos que intentan acceder a el
    acquire(&semaphores[sem].lock);

    // Si el valor del semaforo es 0, el proceso se pone a dormir
    while(semaphores[sem].value == 0) {
        sleep(&semaphores[sem], &semaphores[sem].lock);
    }

    // Cuando el proceso se despierta, decrementa el valor del semaforo
    semaphores[sem].value--;

    //libera el lock
    release(&semaphores[sem].lock);

    return 1;
}

// incrementa el valor del semaforo 'sem', desbloquea los procesos si su valor es cero
//int sem_up(int sem);

int sem_up(int sem) {
    // Verifica si el semáforo es válido y está inicializado
    if(sem < 0 || sem >= MAX_SEM || semaphores[sem].value < 0){
        printf("ERROR: semaforo no valido o no inicializado\n");
        return 0;
    }

    // Toma el lock para garantizar exclusión mutua
    acquire(&semaphores[sem].lock);

    // Incrementa el valor del semáforo
    semaphores[sem].value++;

    // Despierta a un proceso en espera, si lo hay
    wakeup(&semaphores[sem]);

    // Libera el lock
    release(&semaphores[sem].lock);

    return 1;
}