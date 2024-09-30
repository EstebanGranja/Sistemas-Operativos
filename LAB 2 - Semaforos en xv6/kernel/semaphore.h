#include "types.h"
#include "stat.h"
#include "spinlock.h"
#include "../user/user.h"
//#include <stdbool.h>

#define MAX_SEM 128


struct semaphore semaphores[MAX_SEM];
// semaphores[sem] = sem.value  --->  asigna el valor inicial value al semaforo almacenado en la posicion sem


int sem_open(int sem, int value);
// abre y/o inicializa el semaforo 'sem' con el valor arbitrario 'value'


int sem_close(int sem);
// libera la memoria del semaforo 'sem'


int sem_up(int sem);
// incrementa el valor del semaforo 'sem', desbloquea los procesos si su valor es cero


int sem_down(int sem);
// decrementa el valor del semaforo 'sem', bloquea los procesos si su valor es cero