#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "tests/syscall_mock.h"

static const char* builtin[] = {
    "cd",
    "help",
    "exit",
};

static const unsigned int builtin_size = 3;

bool builtin_is_internal(scommand cmd) {                //me tengo que fijar si el comando dado es igual a algun builtin
    assert(cmd != NULL);
    for(unsigned int i = 0;i < builtin_size; i++) {     //recorro los builtin
        if(strcmp(scommand_front(cmd), builtin[i]) == 0) {   //si el comando dado coincide con los builtin devuelve true
            return true;
        }
    }
    return false;                                       //si no, devuelve falso
}

bool builtin_alone(pipeline p) {                        //se fija si la pipeline es de 1 solo elemento y si ese elemento es builtin
    assert(p != NULL);
    bool res = pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
       
    return res;
}

void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));

    if(strcmp(scommand_front(cmd), "cd") == 0) {
        if (scommand_length(cmd) == 1){     //si el comando es cd solo sin argumentos
            chdir(getenv("HOME"));
        }else {
            scommand_pop_front(cmd);
            int res = chdir(scommand_front(cmd));

            if (res == -1){        //si hay error    
                char *str =scommand_to_string(cmd);
                perror(str);
                free(str);
            }
        }
    } else if(strcmp(scommand_front(cmd), "help") == 0) {
        printf("---- myBash -----:\n"
        "\t  Integrantes:\n"
        "\t- Martin Binaghi\n"
        "\t- Ramiro Alvarez\n"
        "\t- Esteban Granja\n"
        "\t- Emilio Lobos\n\n"
        "\t Commands: mybash <command>\n"
        "\t- cd: cambia el directorio actual por el argumento proporcionado.\n"
        "\t- help: muestra por pantalla informacion acerca de los comandos y ayuda adicional.\n"
        "\t- exit: cierra la consola.\n");
    }else if(strcmp(scommand_front(cmd), "exit") == 0){
        exit(EXIT_SUCCESS);
    }
    
}