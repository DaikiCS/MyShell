#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

/* maximum length of user login name    */
#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX  32
#endif

/* maximum length of host name */
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX	256
#endif

#endif
