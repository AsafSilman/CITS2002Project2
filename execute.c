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

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

void runfrompath(char **argv)
{
    char *path_p = getenv("PATH");
    const char s[2] = ":"; //seperator
    char *token;
    char command_buffer[256]; // Buffer to hold command

    char *command = calloc(strlen(argv[0])+2, sizeof(char));
    strcat(command, "/"); 
    strcat(command, argv[0]); 

    token = strtok(path_p, s);
    // iterate through PATH variable
    while(token != NULL)
    {
        strcpy(command_buffer, token);
        strcat(command_buffer, command);
        execv(command_buffer, argv); //if this works, the process image is replaced. execution of loop will stop

        token = strtok(NULL, s);
    }

    free(command);
}

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
    

    if(t == NULL) {			// hmmmm, that's a problem
	exitstatus	= EXIT_FAILURE;
    }
    else if(strcmp(*t->argv, "exit") == 0){
        // If first argument is 'exit', strcmp returns 0
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    else {				// normal, exit commands
        pid_t  pid = fork();
        switch (pid){
            case 0 : // child process
                if (t->argv[0][0] == '/'){
                    // Path Given
                    execv(t->argv[0], t->argv); // attempt to start process
                }
                else{
                    // Search path for command
                    runfrompath(t->argv);
                }
                // If execv failed, and process image did not change
                fprintf(stderr, "%s is not a valid command\n", t->argv[0]);
                exit(EXIT_FAILURE);
                break;
            case -1 : //fork failed
                exitstatus	= EXIT_FAILURE;
                break;
            default : // parent process
                wait( &exitstatus ); //exit status is written once child terminates
                // CAN PRINT THE EXIT CODE HERE
                break;
            }
        }
        
    printf("The exit status was %i\n", exitstatus);
    return exitstatus;
}
