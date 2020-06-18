#include "ssh.h"

#define PROMPT_MAX  (LOGIN_NAME_MAX+PATH_MAX+HOST_NAME_MAX+5)
#define PROMPT_DEF  ">"
char *prompt_sep = PROMPT_DEF;  /* prompt separator str  */
char hostname[HOST_NAME_MAX];   /* computer hostname     */
const char *homePathStr(char *path);
const char *promptStr();

/* control buffer and handler for SIGINT signal capture */
sigjmp_buf ctrlc_buf;
void ctrl_hndlr(int signo) {
    siglongjmp(ctrlc_buf, 1);
}

int main(int argc, char *argv[]) {

    char *line;                     /* readline input buffer */
    pid_t childPID;                 /* command process       */

    command *cs;                    /* command sequence from parse routine        */
    int fdX;                        /* file descriptor to use for all I/O ops     */
    int Pipe[2][2];                 /* used for setting up pipes between commands */
    int pLeft, pRight;              /* used to rotate pipe indexes left and right */
    int pRead=0, pWrite=1;          /* used to index pipe input/output by cmds    */

    /* set up readline */
    rl_bind_key('\t', rl_complete);

    /* get hostname */
    gethostname(hostname, HOST_NAME_MAX);

    /* set up control of SIGINT signal */
    if (signal(SIGINT, ctrl_hndlr) == SIG_ERR) {
        fputs("ERROR: failed to register interrupts in kernel.\n", stderr);
    }
    while (sigsetjmp(ctrlc_buf, 1) != 0) /* empty */;

    /* command-line loop */
    for (;;) {
        /* get command-line */
        line = readline(promptStr());
        if (!line) break;  /* feof(stdin), process ^D */

        /* manage history */
        if (strlen(line)>0) /* don't add blank lines */
            add_history(line);

        /* build command sequence */
        cs = parseline(line);

        /* initialize the initial pipes and set indexes */
        pLeft = 0; pRight = 1;
        if (pipe(Pipe[pLeft])==-1 || pipe(Pipe[pRight])==-1) {
            fprintf(stderr, "ERROR: Pipe failure: %s\n", strerror(errno));
            return (-1);
        }

        for (int i=0; !command_isEmpty(cs[i]); ++i) {
            /* execute command */
            if (command_argn(cs[i])>0) { /* something to execute? */
                /* clean up pipes, close write of previous command */
                close(Pipe[pLeft][pWrite]);

                if (isBuiltIn(command_arg0(cs[i]))) { /* built-in */
                    execBuiltIn(command_args(cs[i]));
                } else { /* not built-in, setup I/O and execute */
                    /* fork a new process */
                    if ((childPID = fork())==0) { /* child process */
                        /* setup I/O for child before exec */
                        /* redirection takes precedence over pipes */
                        if (command_StdIn(cs[i])!=NULL) {
                            fdX = open(command_StdIn(cs[i]), O_RDONLY);
                            dup2(fdX, STDIN_FILENO);
                            close(fdX);
                        } else if (command_isPipedIn(cs[i]))
                            dup2(Pipe[pLeft][pRead], STDIN_FILENO);
                        if (command_StdOut(cs[i])!=NULL) {
                            // check for >>
                            if (command_isAppendOut(cs[i])==1) {
                                fdX = open(command_StdOut(cs[i]), O_WRONLY | O_CREAT | O_APPEND, 0644);
                            }
                            else {
                                fdX = open(command_StdOut(cs[i]), O_WRONLY | O_CREAT | O_TRUNC, 0644); /* TODO: get default value for permissions */
                            }
                            dup2(fdX, STDOUT_FILENO);
                            close(fdX);
                        } else if (command_isPipedOut(cs[i]))
                            dup2(Pipe[pRight][pWrite], STDOUT_FILENO);
                        /* clean up pipes */
                        close(Pipe[pLeft][pRead]);
                        close(Pipe[pLeft][pWrite]);
                        close(Pipe[pRight][pRead]);
                        close(Pipe[pRight][pWrite]);

                        /* ready to load text segment with command program */
                        execvp(command_arg0(cs[i]), command_args(cs[i])); /*, environ);*/

                        /* OOPS! something went wrong! print an error, clean up and exit */
                        fprintf(stderr, "ERROR: Can't execute command '%s': %s\n",
                            command_arg0(cs[i]), strerror(errno));
                        exit(1);
                    } else { /* parent process */
                        /* clean up left pipe after */
                        close(Pipe[pLeft][pRead]);

                        /* wait for child to finish */
                        waitpid(childPID, NULL, 0);
                    }
                }
            }
            /* rotate pipes for next command */
            pLeft = pRight;
            pRight = (pLeft+1)%2;
            if (pipe(Pipe[pRight])==-1) {
                fprintf(stderr, "ERROR: Pipe failure: %s\n", strerror(errno));
                return (-1);
            }

            /* clean up memory before next command */
            command_data_delete(cs[i]);
            command_delete(cs[i]);
        }
        /* clean up IO buffers */
        fflush(stdout);
        fflush(stderr);

        /* clean up pipes */
        close(Pipe[pLeft][pRead]);
        close(Pipe[pLeft][pWrite]);
        close(Pipe[pRight][pRead]);
        close(Pipe[pRight][pWrite]);

        /* clean up memory before next command set */
        // free(line);  /* using history requires us to not free ... yet! */
        free(cs);
    }
    /* EOF (^D) detected, normal exit */
    fputs("exit\n", stdout);

    return 0;
}

/* return a "condensed" path string for '~', among other things */
const char *homePathStr(char *path) {
    static char newpath[PATH_MAX];
    newpath[0] = '\0';
    if (path != NULL) {
        if (strstr(path, getenv("HOME")) == path)
        snprintf(newpath, PATH_MAX, "%c%s", '~', &path[strlen(getenv("HOME"))]);
    else
        snprintf(newpath, PATH_MAX, "%s", path);
    }
    return newpath;
}

/* return prompt string for command loop */
const char *promptStr() {
    static char prompt[PROMPT_MAX];        /* command prompt        */
    snprintf(prompt, PROMPT_MAX, "%s:%s\n%s%s ",
        hostname, homePathStr(getcwd(NULL,0)), getenv("USER"), prompt_sep);
    return prompt;
}
