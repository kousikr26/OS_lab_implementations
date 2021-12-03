
#include "types.h"
#include "stat.h"
#include "user.h"
#include "processInfo.h"
int main(int argc, char *argv[])
{
    int pid;
    if(argc<2){
        pid=getMaxPid();
    }
    else{
        pid = atoi(argv[1]);
    }
    struct processInfo pif;
    
    int retval=getProcInfo(pid, &pif);
    if(retval==-1){
        printf(1, "No process found, return value %d \n",retval );
    }
    else{
        printf(1, "Process Info \n  Parent PID is: %d \n ", pif.ppid);
        printf(1, "Process Size %d \n", pif.psize);
        printf(1, "Number of context switches %d \n", pif.numberContextSwitches);
    }
    
    exit(); 
}