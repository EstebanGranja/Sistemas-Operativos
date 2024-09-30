#include <stdio.h>
#include <glib.h>
#include <assert.h>
#include "strextra.h"
#include "command.h"

struct scommand_s{
    GList *command;
    char *input;
    char *output;
    unsigned int size;
};

/*static bool invrep_scommand(scommand self){
    bool b = true;
    b = b && (self != NULL) && (g_list_length(self->command) == self->size);
    return b;
}*/

scommand scommand_new(){
    scommand new_scommand = malloc(sizeof(struct scommand_s));
    //assert(new_scommand != NULL);
    new_scommand->command = NULL;
    new_scommand->input = NULL;
    new_scommand->output = NULL;
    new_scommand->size = 0;
    //assert(invrep_scommand(new_scommand));
    return new_scommand;
}

scommand scommand_destroy(scommand self){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    g_list_free_full(self->command, (GDestroyNotify)*free);
    self->input = NULL;
    self->output = NULL;
    free(self);
    self = NULL;
    //assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self != NULL && argument != NULL);
    //assert(invrep_scommand(self));
    self->command = g_list_append(self->command, argument);
    self->size++;
    assert(!scommand_is_empty(self));
    //assert(invrep_scommand(self));
}

void scommand_pop_front(scommand self){
    assert(self != NULL && !scommand_is_empty(self));

    free(g_list_first(self->command)->data);
    self->command = g_list_delete_link(self->command, self->command);
    self->size--;
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    self->input = filename;
    //CAPAZ HAY ERROR
    //assert(invrep_scommand(self));
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    self->output = filename;
    //CAPAZ HAY ERROR
    //assert(invrep_scommand(self));
}

bool scommand_is_empty(const scommand self){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    return (self->size == 0);
}

unsigned int scommand_length(const scommand self){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    return (self->size);
}

char * scommand_front(const scommand self){
    assert(self != NULL && !scommand_is_empty(self));
    char *res = self->command->data;
    assert(res != NULL);
    //assert(invrep_scommand(self));
    return res;
}

char * scommand_get_redir_in(const scommand self){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    return self->input;
}

char * scommand_get_redir_out(const scommand self){
    assert(self != NULL);
    //assert(invrep_scommand(self));
    return self->output;
}

char *scommand_to_string(const scommand self) {
    assert(self != NULL);
    //assert(invrep_scommand(self));

    // devuelve un string vacio si no hay comandos
    if (self->size == 0) {
        return strdup("");  
    }

    // inicializa un string buffer para facilitar manejo de memoria
    GString *string = g_string_new("");

    // concateno todos los comandos en self->command
    for (GList *iterator = self->command; iterator != NULL; iterator = iterator->next) {
        g_string_append_printf(string, "%s ", (char*) iterator->data);
    }

    // agrega input y output si es que hay
    if (scommand_get_redir_out(self) != NULL) {
        g_string_append_printf(string, "> %s ", scommand_get_redir_out(self));
    }

    if (scommand_get_redir_in(self) != NULL) {
        g_string_append_printf(string, "< %s ", scommand_get_redir_in(self));
    }

    // conversion a char* y liberacion de memoria
    char *result = g_string_free(string, FALSE);
    return result;
}


// PIPELINE

struct pipeline_s {
    GList *commands;
    bool wait;
    unsigned int size;
};

/*static bool invrep_pipeline(pipeline self){
    bool b = true;
    b = b && (self->commands != NULL) && (g_list_length(self->commands) == self->size);
    return b;
}*/

pipeline pipeline_new(void) {
    pipeline new_pipeline = malloc(sizeof(struct pipeline_s));
    new_pipeline->commands = NULL;
    new_pipeline->wait = true;
    new_pipeline->size = 0;
    //assert(invrep_pipeline(new_pipeline));
    return new_pipeline;
}

static void aux_destroy(scommand self) {
    assert(self != NULL);
    //assert(invrep_scommand(self));
    scommand_destroy(self);
}

pipeline pipeline_destroy(pipeline self) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));
    g_list_free_full(self->commands, (GDestroyNotify)&aux_destroy);
    free(self);
    self = NULL;
    return self;
}

void pipeline_push_back(pipeline self, scommand sc) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));
    self->commands = g_list_append(self->commands, sc);
    self->size++;
    //assert(invrep_pipeline(self));
}

void pipeline_pop_front(pipeline self) {
    assert(self != NULL && !pipeline_is_empty(self));
    //assert(invrep_pipeline(self));
    scommand killme = g_list_first(self->commands)->data;
    self->commands = g_list_remove_link(self->commands, g_list_first(self->commands));
    scommand_destroy(killme);
    self->size--;
    //assert(invrep_pipeline(self));
}

void pipeline_set_wait(pipeline self, const bool w) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));
    self->wait = w;
    //assert(invrep_pipeline(self));
}

bool pipeline_is_empty(const pipeline self) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));
    return (self->size == 0);
}

unsigned int pipeline_length(const pipeline self) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));
    return self->size;
}

scommand pipeline_front(const pipeline self) {
    assert(self != NULL && !pipeline_is_empty(self));
    //assert(invrep_pipeline(self));
    return g_list_first(self->commands)->data;
}

bool pipeline_get_wait(const pipeline self) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));
    return self->wait;
}

char *pipeline_to_string(const pipeline self) {
    assert(self != NULL);
    //assert(invrep_pipeline(self));

    // devuelve string vacio si no hay comandos simples
    if (pipeline_is_empty(self)) {
        return strdup("");  
    }

    // inicializa un string buffer para facilitar manejo de memoria
    GString *string = g_string_new("");

    // recorro cada scommand y lo agrego al string 
    for (GList *iterator = self->commands; iterator != NULL; iterator = iterator->next) {

        // transformo en string el comando donde se encuentra el iterador
        char *command_str = scommand_to_string(iterator->data); 

        // lo agrego al string buffer y libero memoria utilizada
        g_string_append(string, command_str);
        free(command_str); 

        // separo con '|' entre comandos
        if (iterator->next != NULL) {
            g_string_append(string, " | ");
        }
    }

    // agrego '&' al final si wait == false
    if (!pipeline_get_wait(self)) {
        g_string_append(string, " &");
    }

    // conversion a char* y liberacion de memoria
    char *result = g_string_free(string, FALSE);
    return result;
}
