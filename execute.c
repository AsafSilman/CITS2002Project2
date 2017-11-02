#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):             Asaf Silman, Madeleine Lim
   Student number(s):   21985278, 21708238
   Date:                02-11-2017
 */

// -------------------------------------------------------------------

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
 
    if(t == NULL) { // hmmmm, that's a problem
        exitstatus	= EXIT_FAILURE;
    }
    else {
        switch (t->type){
            case CMD_COMMAND    : execute_cmd_command(t, &exitstatus); break;
            case CMD_SEMICOLON  : execute_semicolon_command(t, &exitstatus); break;
            case CMD_AND        : execute_and_command(t, &exitstatus); break;
            case CMD_OR         : execute_or_command(t, &exitstatus); break;
            case CMD_SUBSHELL   : execute_subshell_command(t, &exitstatus); break;
            case CMD_PIPE       : execute_pipe_command(t, &exitstatus); break;
            case CMD_BACKGROUND : execute_background_command(t, &exitstatus); break;
        }
    }
    return exitstatus;   
}

