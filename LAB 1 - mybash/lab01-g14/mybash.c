#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

#include "obfuscated.h"
static char curworkdir[1000];

static void show_prompt(void) {
    char* buf_ptr = getcwd(curworkdir, 1000);
    if (buf_ptr == NULL){
        printf("my_bash> ");
    }
    else
        printf ("\x1b[35m\x1b[20m%s$\x1b[0m ", curworkdir);
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;

    input = parser_new(stdin);
    bool quit = false;

    while (!quit) {
        ping_pong_loop("soXXlab1gXX");
        show_prompt();
        
        pipe = parse_pipeline(input);

        if(pipe!=NULL){
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
        } else{
            exit(EXIT_SUCCESS);
        }
        
        quit = parser_at_eof(input);
    }
    parser_destroy(input); input = NULL;
    printf("\n");
    return EXIT_SUCCESS;
}

