#include "myshell.h"
#include <sys/wait.h>
#include <sys/types.h>
/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
 
    if(t == NULL) { // hmmmm, that's a problem
	    exitstatus	= EXIT_FAILURE;
    }

    switch (t->type){
        case CMD_COMMAND : execute_cmd_command(t, &exitstatus); break;
        case CMD_SEMICOLON : execute_semicolon_command(t, &exitstatus); break;
        case CMD_AND: break;
        case CMD_OR: break;
        case CMD_SUBSHELL: break;
        case CMD_PIPE: break;
        case CMD_BACKGROUND: break;
    }

    return exitstatus;   
}

