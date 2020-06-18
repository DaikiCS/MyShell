#ifndef __SSH_H__
#define __SSH_H__

#include "common.h"

#include <sys/wait.h>
#include <setjmp.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <errno.h>

#include "parse.h"
#include "builtins.h"

extern char **environ;

#endif
