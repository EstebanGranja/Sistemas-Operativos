#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <glib.h>
#include <signal.h>
#include <stdbool.h>
#include "command.h"
#include "tests/syscall_mock.h"

static void execute_command(scommand scommand) {
    // inicializo un array de strings que contendra el scommand
    unsigned int length = scommand_length(scommand);
    char **argv = malloc(sizeof(char *)*(length + 1)); 

    // recorro el scommand guardando cada parametro del comando
    unsigned int i = 0;
    while (!scommand_is_empty(scommand)) {
        // reservo memoria segun la longitud del string y lo guardo en su respectiva posicion del array
        unsigned int command_size = strlen(scommand_front(scommand)) + 1; // 
        argv[i] = malloc(sizeof(char)*command_size);        
        strcpy(argv[i], scommand_front(scommand));               
        scommand_pop_front(scommand);
        i++;
    }
    // este punto scommand esta guardado en el array argv

    // ahora tengo en cuenta si algun parametro in/out esta presente en el comando
    char* command_in = scommand_get_redir_in(scommand);
    if (command_in != NULL) {
        int in_redir = open(command_in, O_RDONLY, S_IRWXU);

        // si hay algun error en la apertura del archivo
        if(in_redir < 0){
            perror("Open");
            return;
        }

        // desactiva stdin, redirige in_redir como entrada, cierra el archivo 
        close(STDIN_FILENO);   
        dup2(in_redir, STDIN_FILENO);
        close(in_redir);
    }
    
    char* command_out = scommand_get_redir_out(scommand);
    if (command_out != NULL) {
        // crea el archivo si no existe (con los permisos S_IRWXU | modo solo escritura | borra lo que tenia si ya existia
        int out_redir = open(command_out, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU); 

        // si hay algun error en la apertura del archivo
        if(out_redir < 0){
            perror("Open");
            return;
        }

        // desactiva stdout, redirige out_redir, cierra el archivo
        close(STDOUT_FILENO);
        dup2(out_redir, STDOUT_FILENO);
        close(out_redir);
    }
    
    // deja argv[] en un formato usable para execvp
    argv[length] = NULL;

    int res = execvp(argv[0], argv);
    
    if (res == -1) {
        perror("syscall");
        return;
    }
}

void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);

    if(pipeline_is_empty(apipe)){
        return;
    }

    if(builtin_alone(apipe)){
        builtin_run(pipeline_front(apipe));
        return;
    }

    if(!pipeline_get_wait(apipe)){
        signal(SIGCHLD, SIG_IGN);
    }

    unsigned int length = pipeline_length(apipe);

    int* chld_pids = malloc(length * sizeof(int));

    int filedes[2];
    int aux[2];

    for(unsigned int i = 0; i < length; ++i){
        if(i != 0){
            aux[0] = filedes[0];
            aux[1] = filedes[1];
        }

        if(i != length - 1){
            pipe(filedes);
        }

        int rc = fork();

        if (rc < 0) {
            fprintf(stderr, "FORK FAILED.\n");
            return;
        } else if (rc == 0) {

            if (i != length - 1) {
                close(filedes[0]);
                close(STDOUT_FILENO);
                dup2(filedes[1], STDOUT_FILENO);
                close(filedes[1]);
            }

            if (i != 0) {
                close(aux[1]);
                close(STDIN_FILENO);
                dup2(aux[0], STDIN_FILENO);
                close(aux[0]);
            }

            char* command_str = scommand_to_string(pipeline_front(apipe));
            execute_command(pipeline_front(apipe));
            fprintf(stderr, "Error executing: %s\n", command_str);
            exit(EXIT_FAILURE);
        } else {
            if (i != 0) {
                close(aux[0]);
                close(aux[1]);
            }
            chld_pids[i] = rc;
            pipeline_pop_front(apipe);
        }
    }

    if (pipeline_get_wait(apipe)) {
        for (unsigned int i = 0; i < length; ++i)
            waitpid(chld_pids[i], NULL, 0);
    }
  
    free(chld_pids);
}