#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):             Asaf Silman, Madeleine Lim
   Student number(s):   21985278, 21708238
   Date:                02-11-2017
 */

//  THREE INTERNAL VARIABLES (SEE myshell.h FOR EXPLANATION)
char	*HOME, *PATH, *CDPATH;

char	*argv0		= NULL;		// the program's name
bool	interactive	= false;

// ------------------------------------------------------------------------

pid_t BACKGROUND_PROCESSES[MAX_BG_PROCESSES];
int   *num_background_processes; // Count of current number of background processes


void check_allocation0(void *p, char *file, const char *func, int line)
{
    if(p == NULL) {
	fprintf(stderr, "%s, %s() line %i: unable to allocate memory\n",
			file, func, line);
	exit(2);
    }
}

static void print_redirection(SHELLCMD *t)
{
    if(t->infile != NULL)
	printf("< %s ", t->infile);
    if(t->outfile != NULL) {
	if(t->append == false)
	    printf(">");
	else
	    printf(">>");
	printf(" %s ", t->outfile);
    }
}

void print_shellcmd0(SHELLCMD *t)
{
    if(t == NULL) {
	printf("<nullcmd> ");
	return;
    }

    switch (t->type) {
    case CMD_COMMAND :
	for(int a=0 ; a<t->argc ; a++)
	    printf("%s ", t->argv[a]);
	print_redirection(t);
	break;

    case CMD_SEMICOLON :
	print_shellcmd0(t->left); printf("; "); print_shellcmd0(t->right);
	break;

    case CMD_AND :
	print_shellcmd0(t->left); printf("&& "); print_shellcmd0(t->right);
	break;

    case CMD_OR :
	print_shellcmd0(t->left); printf("|| "); print_shellcmd0(t->right);
	break;

    case CMD_SUBSHELL :
	printf("( "); print_shellcmd0(t->left); printf(") ");
	print_redirection(t);
	break;

    case CMD_PIPE :
	print_shellcmd0(t->left); printf("| "); print_shellcmd0(t->right);
	break;

    case CMD_BACKGROUND :
	print_shellcmd0(t->left); printf("& "); print_shellcmd0(t->right);
	break;

    default :
	fprintf(stderr, "%s: invalid CMDTYPE in print_shellcmd0()\n", argv0);
	exit(EXIT_FAILURE);
	break;
    }
}

// ------------------------------------------------------------------------

void run_cmd(int *exitstatus, SHELLCMD *t)
{
    /* Step 1 Execute commands */
    pid_t  pid = fork();
    switch (pid){
        case 0 : // child process
            if (t->infile != NULL) {
                execute_infile(t);
            }
            
            if (t->outfile != NULL ) {
                execute_outfile(t);
            }

            if (t->argv[0][0] == '/' || t->argv[0][0] == '.'){
                // Path Given
                execv(t->argv[0], t->argv); // attempt to start process
                run_shellscript(t->argv); // attempt to start shellscript
            }
            else{
                // Search path for command
                search_path_run(t->argv);
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
            *exitstatus=  WEXITSTATUS(*exitstatus);
			break;
    }
}

void search_path_run(char **argv)
{
    /* Step 2 Search path for the command specified */
    char *path_p = getenv("PATH");
    const char s[2] = ":"; //separator
    char *token;
    char command_buffer[MAXPATHLEN]; // Buffer to hold command

    char *command = calloc(strlen(argv[0])+2, sizeof(char));
    strcat(command, "/"); 
    strcat(command, argv[0]); 

    token = strtok(path_p, s);
    // Iterate through PATH variable
    while(token != NULL)
    {
        strcpy(command_buffer, token);
        strcat(command_buffer, command);
        execv(command_buffer, argv); 
        //if this works, the process image is replaced. execution of loop will stop
        token = strtok(NULL, s);
    }

    free(command);
}

void execute_infile(SHELLCMD *t)
{
    /* Step 6 Redirects standard input to file specified in SHELLCMD */

    int ifd = open(t->infile, O_RDONLY);
    dup2(ifd, STDIN_FILENO);
    close(ifd);
}

void execute_outfile(SHELLCMD *t)
{
    /* Step 6 Redirects output to file specified in SHELCMD.
      This function works for appending, truncating file data 
      and creating a new file if it did not previously exist */
       
    int out;
    if (t->append) {
        out = open(t->outfile, O_WRONLY|O_CREAT|O_APPEND, O_RDWR);        
    }
    else {
        out = open(t->outfile, O_WRONLY|O_CREAT|O_TRUNC, O_RDWR);
    }
    dup2(out, STDOUT_FILENO);
}

void run_shellscript(char **argv)
{
    /* Step 8 Shell script execution
      To be used only in run_cmd */

    int access_status = access(argv[0], R_OK);
    if (access_status != 0) { return; }
    
    FILE *script = fopen(argv[0], "r");
    
    char *t_argv[2] = {argv0, NULL};
    
    dup2(fileno(script), STDIN_FILENO);
    fclose(script);
    
    execv(argv0, t_argv); // start myshell
    perror("Failed to start myshell process");
    exit(0); //kills child process in run_cmd
}

void background_command_handler(int sig)
{
    /* Step 9 Background Execution 
       This is the handler function for signal */
    pid_t child_id = wait(NULL);
    remove_background_processes(child_id);
    printf("Process ID %d has finished\n", child_id);
}

void add_background_processes(pid_t pid)
{
    /* Step 9 Background Execution
    Function to keep a log of processes */
    BACKGROUND_PROCESSES[*num_background_processes] = pid;
    ++*num_background_processes;
}

void remove_background_processes(pid_t pid)
{
    /* Step 9 Background Execution
       Function to remove completed processes */

    int num = *num_background_processes;
    for (int i=0; i<num; ++i) {
        if (BACKGROUND_PROCESSES[i] == pid) {
            BACKGROUND_PROCESSES[i] = BACKGROUND_PROCESSES[num-1];
            BACKGROUND_PROCESSES[num-1] = 0;
            break;
        }
    }
    --*num_background_processes;
}

void kill_background_processes(void)
{
    int num = *num_background_processes;
    for (int i=0; i<num; ++i) {
        kill(BACKGROUND_PROCESSES[i], SIGKILL);
    }
    for (int i=0; i<num; ++i) {
        waitpid(BACKGROUND_PROCESSES[i], NULL, 0);
        printf("Killed process %i\n", BACKGROUND_PROCESSES[i]);
    }
}
