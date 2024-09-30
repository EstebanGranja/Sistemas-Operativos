#include <stdlib.h>
#include <stdbool.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    bool startispipe;
    bool startisbackground;

    parser_skip_blanks(p);
    parser_op_pipe(p, &startispipe);
    if (startispipe) {
        printf("Error en la utilizacion del operador |");
        return NULL;
    }

    parser_op_background(p, &startisbackground);
    if (startisbackground){
        printf("Error en la utilizacion del operador &");
        return NULL;
    };
    
    scommand new_command = scommand_new();
    arg_kind_t type;
    char* next_arg;
    bool endfile = parser_at_eof(p); 

    while (!endfile) {
        next_arg = parser_next_argument(p, &type); 

        if (next_arg != NULL) {
            if (type == ARG_NORMAL) 
            {
               scommand_push_back(new_command, next_arg);
            } else if (type == ARG_INPUT) 
            {
                scommand_set_redir_in(new_command, next_arg);
            } else if (type == ARG_OUTPUT) 
            {
                scommand_set_redir_out(new_command, next_arg);
            }
        } else if (type == ARG_OUTPUT || type == ARG_INPUT ) {
            printf("Error: output o input invalido\n");
            exit(EXIT_FAILURE);
            return NULL;
        } else {break;}
    }

    return new_command;
}
    
pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;     

    while (another_pipe && !error) {
        cmd = parse_scommand(p);
        error = (cmd==NULL || scommand_is_empty (cmd)); 
        
        parser_skip_blanks(p);                                 
        pipeline_push_back(result, cmd);                        
        parser_op_pipe(p, &another_pipe);                       
    }

    if (error) {
        printf("Error: output o input invalido.\n");
        result = pipeline_destroy (result);              
        return NULL;
    }

    bool is_background;
    parser_op_background(p, &is_background);
    pipeline_set_wait(result, !is_background);
    
    bool gb_left;
    parser_garbage(p, &gb_left);

    if (gb_left) {
        printf("Error: comando invalido.\n");
        pipeline_destroy(result);
        return NULL;
    }

    return result; 
}

