#include <syslog.h>			  /* syslog */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"
#include "typesdef.h"          /* type definitions and include of global configuration */
#include "command.h"

#include "swb-serial.h"		   /* swb-bus functions */
#include "smi-serial.h"		   /* swb-bus functions */

int handleCommand(void) {


    // clear command-buffer
    command[0].id = -1;
    command[0].group = -1;
    command[0].command = -1;
    return(EXIT_SUCCESS);
}
