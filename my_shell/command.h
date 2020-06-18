#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "array.h"

struct command {
    char *filenameIN;               /* filename == NULL if no redirection */
    char appendOUT, *filenameOUT;
    char pipeIN, pipeOUT;
    array args;
};

typedef struct {
    struct command *ptr;
} command;

/* allocation/deallocation of command data structure */
command command_new();
void command_delete(command);
int command_isEmpty(command);
void command_data_delete(command);

/* args property methods for command */
char **command_args(command);
char *command_arg0(command);
int command_argn(command);
void *command_get_arg(command, int);
void *command_set_arg(command, int, void*);
int command_push_arg(command, void*);

/* redirection properties for command */
int command_isRedirectedIO(command);
char *command_StdIn(command);
int command_redirectIn(command, char*);
char *command_StdOut(command);
int command_redirectOut(command, char*);
int command_set_appendOut(command);
int command_unset_appendOut(command);
int command_isAppendOut(command);

/* pipe properties for command */
int command_isPipedIO(command);
int command_isPipedIn(command);
int command_set_pipeIn(command);
int command_unset_pipeIn(command);
int command_isPipedOut(command);
int command_set_pipeOut(command);
int command_unset_pipeOut(command);

/* print out contents of command */
void command_dump(command);

#endif
