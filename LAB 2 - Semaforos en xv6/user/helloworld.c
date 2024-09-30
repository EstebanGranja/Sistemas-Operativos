#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    hello(); // Llamada a la syscall
    exit(0);
}
