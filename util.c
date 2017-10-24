#include "myshell.h"
#include "util.h"

#define FILE_ACCESS 0600

void run_cmd(int *exitstatus, SHELLCMD *t)
{

    pid_t  pid = fork();
    switch (pid){
        case 0 : // child process
            if (t->infile != NULL) {
                execute_infile(t);
            }
    
            if (t->outfile != NULL ) {
                execute_outfile(t);
            }

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

void execute_infile(SHELLCMD *t)
{
    int ifd = open(t->infile, O_RDONLY);
    dup2(ifd, STDIN_FILENO);
    close(ifd);
}

void execute_outfile(SHELLCMD *t)
{
    /* This function works for appending, for truncating file data 
       and for creating a new file if did not previously exist */

    int out;

    if (t->append) {
        out = open(t->outfile, O_WRONLY|O_CREAT|O_APPEND, FILE_ACCESS);        
    }
    else {
        out = open(t->outfile, O_WRONLY|O_CREAT|O_TRUNC, FILE_ACCESS);
    }
    dup2(out, STDOUT_FILENO);
    fflush(stdout); 
    close(out);
}
