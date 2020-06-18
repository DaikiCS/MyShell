#include "common.h"
#include "command.h"

command command_new() {
    command c;
    c.ptr = malloc(sizeof(struct command)); /* create header for data structure */
    if (c.ptr!=NULL) {
        c.ptr->appendOUT = 0; /* false */
        c.ptr->filenameIN = c.ptr->filenameOUT = NULL;
        c.ptr->pipeIN = c.ptr->pipeOUT = 0; /* false */
        c.ptr->args = array_new(1);
    }
    return c;
}

void command_args_delete(command c) {
    if (c.ptr!=NULL)
        array_data_delete(c.ptr->args);
}

void command_delete(command c) {
    if (c.ptr!=NULL) {
        array_delete(c.ptr->args);
        free(c.ptr);
    }
}

int command_isEmpty(command c) {
    return c.ptr==NULL;
}

void command_data_delete(command c) {
    if (c.ptr!=NULL) {
        if (c.ptr->filenameIN!=NULL)
            free(c.ptr->filenameIN);
        if (c.ptr->filenameOUT!=NULL)
            free(c.ptr->filenameOUT);
        array_data_delete(c.ptr->args);
    }
}

char **command_args(command c) {
    void **args = NULL;
    if (c.ptr!=NULL && c.ptr->args.ptr!=NULL)
        args = c.ptr->args.ptr->data;
    return args;
}

char *command_arg0(command c) {
    void *arg0 = NULL;
    if (c.ptr!=NULL && c.ptr->args.ptr!=NULL)
        arg0 = c.ptr->args.ptr->data[0];
    return arg0;
}

int command_argn(command c) {
    int argn = -1;
    if (c.ptr!=NULL)
        argn = array_get_size(c.ptr->args);
    return argn;
}

void *command_get_arg(command c, int i) {
    void *arg = NULL;
    if (c.ptr!=NULL)
        arg = array_get(c.ptr->args, i);
    return arg;
}

void *command_set_arg(command c, int i, void *p) {
    void *q = NULL;
    if (c.ptr!=NULL && i<array_get_size(c.ptr->args)) {
        q = array_get(c.ptr->args, i);
        array_set(c.ptr->args, i, p);
    }
    return q;
}

int command_push_arg(command c, void *p) {
    int retval = -1;
    if (c.ptr!=NULL)
        retval = array_push(c.ptr->args, p);
    return retval;
}

int command_isRedirectedIO(command c) {
    int retval = 0;
    if (c.ptr!=NULL)
        retval = (c.ptr->filenameIN!=NULL || c.ptr->filenameOUT!=NULL);
    return retval;
}

char *command_StdIn(command c) {
    char *retval = NULL;
    if (c.ptr!=NULL)
        retval = c.ptr->filenameIN;
    return retval;
}

int command_redirectIn(command c, char *name) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->filenameIN = name;
    }
    return retval;
}

char *command_StdOut(command c) {
    char *retval = NULL;
    if (c.ptr!=NULL)
        retval = c.ptr->filenameOUT;
    return retval;
}

int command_redirectOut(command c, char *name) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->appendOUT = 0;
        c.ptr->filenameOUT = name;
    }
    return retval;
}

int command_set_appendOut(command c) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->appendOUT = 1;
    }
    return retval;
}

int command_unset_appendOut(command c) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->appendOUT = 0;
    }
    return retval;
}

int command_isAppendOut(command c) {
    int retval = 0;
    if (c.ptr!=NULL)
        retval = c.ptr->appendOUT;
    return retval;
}

int command_isPipedIO(command c) {
    int retval = 0;
    if (c.ptr!=NULL)
        retval = (c.ptr->pipeIN || c.ptr->pipeOUT);
    return retval;
}

int command_isPipedIn(command c) {
    int retval = 0;
    if (c.ptr!=NULL)
        retval = c.ptr->pipeIN;
    return retval;
}

int command_set_pipeIn(command c) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->pipeIN = 1;
    }
    return retval;
}

int command_unset_pipeIn(command c) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->pipeIN = 0;
    }
    return retval;
}

int command_isPipedOut(command c) {
    int retval = 0;
    if (c.ptr!=NULL)
        retval = c.ptr->pipeOUT;
    return retval;
}

int command_set_pipeOut(command c) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->pipeOUT = 1;
    }
    return retval;
}

int command_unset_pipeOut(command c) {
    int retval = -1;
    if (c.ptr!=NULL) {
        retval = 0;
        c.ptr->pipeOUT = 0;
    }
    return retval;
}

void command_dump(command c) {
    if (c.ptr!=NULL) {
        fprintf(stdout, "COMMAND\n");
        fprintf(stdout, "redirection:\n\tinput : %s\n", c.ptr->filenameIN!=NULL?c.ptr->filenameIN:"NULL");
        fprintf(stdout, "\toutput: %s\n", c.ptr->filenameOUT!=NULL?c.ptr->filenameOUT:"NULL");
        fprintf(stdout, "pipes:\n\tinput : %s\n", c.ptr->pipeIN?"yes":"no");
        fprintf(stdout, "\toutput: %s\n", c.ptr->pipeOUT?"yes":"no");
        fprintf(stdout, "args ");
        array_dump(c.ptr->args);
    } else
        fprintf(stdout, "command: NULL\n");
}
