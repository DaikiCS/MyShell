#include "common.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

/* maximum length of built-in commands  */
#define COMMAND_MAX     50

/* all built-in commands use a BuiltInFunction for exec */
typedef void (*BuiltInFunction)(char **);

/* structure for internal organization of a built-in command */
typedef struct {
    char name[COMMAND_MAX]; /* command - actual name of built-in     */
    BuiltInFunction exec;   /* function to perform command's actions */
    char *usage;            /* usage information for the built-in    */
    char *descr;            /* help description of the built-in      */
} BuiltInCommand;

void execCD(char *args[]) {
    int err = 0;
    char path[PATH_MAX];
    if (args[1]==NULL)
        snprintf(path, PATH_MAX, "%s", getenv("HOME"));
    else if (args[1][0]=='~')
        snprintf(path, PATH_MAX, "%s%s", getenv("HOME"), &args[1][1]);
    else
        snprintf(path, PATH_MAX, "%s", args[1]);
    err = chdir(path);
    if (err<0)
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
}

void execEXIT(char *args[]) {
    exit(0);
}

/* print all environmental variables */
void execPRINTENV() { 
    fprintf(stdout, "%d\n", system("env"));
}

/* prototype for the help function for all built-ins */
void execHELP();

/* prototypes */
void execPRINT();
void execSET();
void execMD();
void execRD();
void execDEL();

/* all built-in commands in system */
/* Enter each as:
 *  { "command", BuiltInFunction, "usage", "description" }
 */
BuiltInCommand BuiltInCommands[] = {
    {
        "cd",       execCD,
        "cd <path>",        "change directory to <path>"
    },{
        "exit",     execEXIT,
        "exit",             "exit the shell"
    },{
        "help",     execHELP,
        "help",             "print this menu"
    },{
        "print",       execPRINT,
        "print <VAR>",        "print value of environment variable <VAR>"
    },{
        "set",       execSET,
        "set <VAR> <VALUE>",  "set value of environment variable <VAR> to <VALUE>"
    },{
        "printenv",       execPRINTENV,
        "printenv",        "print the environment (all variables and values)"
    },{
        "md",       execMD,
        "md <PATH>",        "create directory given by <PATH>"
    },{
        "rd",       execRD,
        "rd <PATH>",        "remove directory given by <PATH>"
    },{
        "del",       execDEL,
        "del <FILE>",       "delete <FILE>, if directory, recursively deletes"
    }
};

/* count of all built-in commands in system */
int BuiltInCount = sizeof(BuiltInCommands)/sizeof(BuiltInCommand);

/* help function for all built-in functions */
void execHELP() {
    const char *BuiltInHelp(int);

    fprintf(stdout, "Built-in commands:\n");
    for (int i=0;i<BuiltInCount;++i)
        fprintf(stdout, "\t%s\n", BuiltInHelp(i));
}

/* print function to print environmental variable */
void execPRINT(char *args[]) {
    if (args[1]==NULL)
        fprintf(stdout, "Error: usage-> print <VAR>\n");
    else {
        fprintf(stdout, "%s\n", getenv(args[1]));
    }
}

/* set function to set environmental variable */
void execSET(char *args[]) {
    if (args[1]==NULL || args[2]==NULL)
        fprintf(stdout, "Error: usage-> set <VAR> <VALUE>\n");
    else {
        setenv(args[1], args[2], 1);
    }
}

/* md function to make a directory */
void execMD(char *args[]) {
    if (args[1]==NULL)
        fprintf(stdout, "Error: usage-> md <PATH>\n");
    else
        mkdir(args[1], 0700);
}

/* md function to remove an empty directory */
void execRD(char *args[]) {
    if (args[1]==NULL)
        fprintf(stdout, "Error: usage-> rd <PATH>\n");
    else 
        if (rmdir(args[1]) < 0)
            fprintf(stdout, "Error: directory '%s' not empty\n", args[1]);
}

/* md function to remove an empty directory */
void execDEL(char *args[]) {
    if (args[1]==NULL)
        fprintf(stdout, "Error: usage-> del <FILE>\n");
    else {
        /* check for a file or directory */
        struct stat path_stat;
        stat(args[1], &path_stat);
        int f_d = S_ISREG(path_stat.st_mode);

        char option[1]; // user input
        fprintf(stdout, "Are you sure you want to delete 'a.out'? (y/n) ");
        scanf("%s", option);
        // text
        if (f_d == 1 && option[0] == 'y') {
            remove(args[1]);
        }
        // directory
        else if (f_d == 0 && option[0] == 'y') {
            char rmv_d[100] = "";
            strcat(rmv_d, "rm -rf ");
            strcat(rmv_d, args[1]);
            system(rmv_d);
        }
    }
}

/* return index+1 in the builtInCmds array or 0 for not found */
int isBuiltIn(char *cmd) {
    int i=-1;
    if (cmd!=NULL)
        for (i = 0;i<BuiltInCount;++i)
            if (strncmp(cmd,BuiltInCommands[i].name,COMMAND_MAX)==0)
                break;
    return i<BuiltInCount?i+1:0;
}

/* i is the index or SEARCH_FOR_CMD */
void execBuiltIn(char *args[]) {
    int i = isBuiltIn(args[0])-1;
    if (i>-1 && i<BuiltInCount)      /* found a valid index */
        BuiltInCommands[i].exec(args);
    else                             /* not a built-in command */
        fprintf(stderr, "ERROR: unknown built-in command\n");
}

/* utility functions for the built-in exec functions */

#define HELP_MAX    255
/* return immutable string for help message or NULL on error */
const char *BuiltInHelp(int i) {
    static char buffer[HELP_MAX]; /* buffer for help message */
    const char *retval = buffer;  /* set return value to point to buffer */

    /* initialize to the empty string, so if pointer */
    /* exists outside of function this will reset it */
    buffer[0] = '\0';

    if (0<=i && i<BuiltInCount)
        snprintf(buffer, HELP_MAX, "%-15s - %s", BuiltInCommands[i].usage, BuiltInCommands[i].descr);
    else
        retval = NULL;

    return retval;
}
