// Header file for commonly used functions
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern void runfrompath(char**);
extern void run_cmd(int*, SHELLCMD*);
extern void execute_infile(SHELLCMD*);
extern void execute_outfile(SHELLCMD*);

