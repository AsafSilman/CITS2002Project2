#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):             Asaf Silman, Madeleine Lim
   Student number(s):   21985278, 21708238
   Date:                date-of-submission
 */

int main(int argc, char *argv[])
{

//  REMEMBER THE PROGRAM'S NAME (TO REPORT ANY LATER ERROR MESSAGES)
    argv0	= (argv0 = strrchr(argv[0],'/')) ? argv0+1 : argv[0];
    argc--;				// skip 1st command-line argument
    argv++;

//  INITIALIZE THE THREE INTERNAL VARIABLES
    HOME	= getenv("HOME");
    if(HOME == NULL) {
	HOME	= DEFAULT_HOME;
    }

    PATH	= getenv("PATH");
    if(PATH == NULL) {
	PATH	= DEFAULT_PATH;
    }

    CDPATH	= getenv("CDPATH");
    if(CDPATH == NULL) {
	CDPATH	= DEFAULT_CDPATH;
    }

    // SETUP BACKGROUND PROCESS MANAGEMENT
    num_background_processes = malloc(sizeof(pid_t));
    *num_background_processes = 0;

//  DETERMINE IF THIS SHELL IS INTERACTIVE
    interactive		= (isatty(fileno(stdin)) && isatty(fileno(stdout)));

    int exitstatus	= EXIT_SUCCESS;

//  READ AND EXECUTE COMMANDS FROM stdin UNTIL IT IS CLOSED (with control-D)
    while(!feof(stdin)) {
	SHELLCMD	*t = parse_shellcmd(stdin);

	if(t != NULL) {

//  WE COULD DISPLAY THE PARSED COMMAND-TREE, HERE, BY CALLING:
//	    print_shellcmd(t);

        exitstatus = execute_shellcmd(t); 
	    free_shellcmd(t);
	}
    }
    if(interactive) {
	fputc('\n', stdout);
    }
    kill_background_processes();

    return exitstatus;
}
