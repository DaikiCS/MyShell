#include "common.h"
#include <string.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

/* process command-line by returning a "chunk" of text each call */
char *getChunk(char *line) {
    static char *ch=NULL;
    char q; int qi;
    char *buffer; int size=20, i=0;
    if (line != NULL)
        ch = line;
    if (ch==NULL || *ch=='\0')
        return NULL;
    buffer = malloc(size*sizeof(char));
    while (isspace(*ch)) ++ch;

    while (!isspace(*ch) && *ch!='\0') {
        if (*ch == '>') {
            if (i == 0) {
                buffer[i++] = *ch++;
                if (*(ch) == '>')
                    buffer[i++] = *ch++;
            }
            break;
        } else if (*ch == '<') {
            if (i == 0)
                buffer[i++] = *ch++;
            break;
        } else if (*ch == '|') {
            if (i == 0)
                buffer[i++] = *ch++;
            break;
        } else if (*ch == ';') {
            if (i == 0)
                buffer[i++] = *ch++;
            break;
        } else if (*ch == '\\') {
            buffer[i++] = *++ch;
            ++ch;
        } else if (*ch=='"' || *ch=='\'') {
            q = *ch;
            ++ch;
            while (*ch!='\0' && *ch!=q) {
                if (*ch=='\\') {
                    ++ch;
                    switch (*ch) {
                        case '0':   /* NUL */
                            buffer[i++] = 0x00;
                            break;
                        case 'a':   /* BEL */
                            buffer[i++] = 0x07;
                            break;
                        case 'b':   /* BS */
                            buffer[i++] = 0x08;
                            break;
                        case 't':   /* HT */
                            buffer[i++] = 0x09;
                            break;
                        case 'n':   /* LF */
                            buffer[i++] = 0x0A;
                            break;
                        case 'v':   /* VT */
                            buffer[i++] = 0x0B;
                            break;
                        case 'f':   /* FF */
                            buffer[i++] = 0x0C;
                            break;
                        case 'r':   /* CR */
                            buffer[i++] = 0x0D;
                            break;
                        case 'e':   /* ESC */
                            buffer[i++] = 0x1B;
                            break;
                        default:
                            buffer[i++] = *ch;
                    }
                    ++ch;
                } else
                    buffer[i++] = *ch++;
            }
            if (*ch=q)
                ++ch;
        } else
            buffer[i++] = *ch++;
    }
    if (i == 0) {
        free(buffer);
        buffer = NULL;
    } else
        buffer[i] = '\0';

    return buffer;
}

/* parse the command-line, return cmds array with redirection and piping */
/* Sample input command-line:
 *   cat *.c>output<input a\ bc"abc"abc\"abc\"; >>out test x 'a b'cd|"l"s /bin ;
 * which becomes the following 'chunks' with space represented by _ in chunks:
 *   cat *.c > output < input a_bcabcabc"abc" ; >> out test a_bcd | ls /bin ;
*/
command *parseline(char *line) {
    command *commands;

    array cmds;   /* array of commands found on command-line  */
    command cmd;  /* current command that is being built      */
    char *arg;    /* current argument of the current command  */

    /* initialize data structures */
    cmds = array_new(1);
    cmd = command_new();

    arg = getChunk(line);
    while (arg!=NULL) {

        if (strcmp(">", arg)==0) {
            free(arg);
            arg = getChunk(NULL);
            if (arg==NULL)
                fprintf(stderr,"ERROR: redirection: no filename for output\n");
            else {
                command_redirectOut(cmd, arg);
            }
        } else if (strcmp(">>", arg)==0) {
            free(arg);
            arg = getChunk(NULL);
            if (arg==NULL)
                fprintf(stderr,"ERROR: redirection: no filename for appending output\n");
            else {
                command_redirectOut(cmd, arg);
                command_set_appendOut(cmd);
            }
        } else if (strcmp("<", arg)==0) {
            free(arg);
            arg = getChunk(NULL);
            if (arg==NULL)
                fprintf(stderr,"ERROR: redirection: no filename for input\n");
            else {
                command_redirectIn(cmd, arg);
            }
        } else if (strcmp("|", arg)==0) {
            free(arg);
            if (command_argn(cmd)==0)
                fprintf(stderr, "ERROR: pipe: no input source for pipe\n");
            else {
                command_set_pipeOut(cmd);       /* set up the output pipe */
                command_push_arg(cmd, NULL);    /* terminate the args list */
                array_push(cmds, cmd.ptr);      /* push the current command to cmds array */
                cmd = command_new();            /* start new command */
                command_set_pipeIn(cmd);        /* set up the input pipe */
            }
        } else if (strcmp(";", arg)==0) {
            free(arg);
            if (command_argn(cmd)>0) {
                command_push_arg(cmd, NULL);    /* terminate the args list */
                array_push(cmds, cmd.ptr);      /* push the current command to cmds array */
                cmd = command_new();            /* start new command */
            } else if (command_isPipedOut(cmd))
                fprintf(stderr, "ERROR: pipe: no output source for pipe\n");
        } else
            command_push_arg(cmd, arg);

        arg = getChunk(NULL);
    }
    if (command_argn(cmd)>0) {
        command_push_arg(cmd, NULL);
        array_push(cmds, cmd.ptr);
    } else {
        if (command_isPipedOut(cmd))
            fprintf(stderr, "ERROR: pipe: no output source for pipe\n");
        command_delete(cmd);
    }

    array_push(cmds, NULL);

    commands = (command *)cmds.ptr->data;
    free(cmds.ptr);

    return commands;
}
