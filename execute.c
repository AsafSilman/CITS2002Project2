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

//TODO Docstring

//LOCAL DEFINITIONS
void execute_cmd(int*, SHELLCMD*);

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
    else if(strcmp(t->argv[0], "exit") == 0){
        // If first argument is 'exit', strcmp returns 0
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(t->argv[0], "cd") == 0){
        // If first argument is 'cd', strcmp returns 0
        if(t->argc==1)
        {
            chdir(HOME);
        }
        else
        {
            chdir(t->argv[1]);
        }
        exitstatus = 0;
    }
    else if(strcmp(t->argv[0], "time") == 0){
        // If first argument is 'exit', strcmp returns 0    
        struct timeval  start_time;
        struct timeval  end_time;
        
        (t->argv)++; // increment argv by 1
        (t->argc)--; // decrement argc by 1

        gettimeofday( &start_time, NULL );
        execute_cmd( &exitstatus, t );
        (t->argv)--; // increment argv by 1        
        gettimeofday( &end_time, NULL );

        if (exitstatus == 0){ // 
            int execution_time = end_time.tv_usec - start_time.tv_usec;
            fprintf(stderr, "%i ms\n", execution_time/1000 ); // convert to milliseconds
        }
        exitstatus = 0;
    }
    else {				// normal, exit commands
        execute_cmd(&exitstatus, t);
    }
    return exitstatus;
}

void execute_cmd(int *exitstatus, SHELLCMD *t){
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
            *exitstatus	= EXIT_FAILURE;
            break;
        default : // parent process
            wait( exitstatus ); //exit status is written once child terminates
            // CAN PRINT THE EXIT CODE HERE
            break;
    }
}
