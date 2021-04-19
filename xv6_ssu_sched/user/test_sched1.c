#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char** argv){
    int pid;
    int mypid;
    
    //change the priority of init processes.
    setnice(1,10);

    //change the priority of current processes.
    setnice(getpid(), 2);

    //create a child process
    pid = fork();

    if(pid == 0){
        printf(1, "#### state 2 ####\n");
    }else{
        printf(1, "#### state 1 ####\n");

        //change the priority of parent processes.
        setnice(pid, 10);
        wait(); // Yield CPU

        printf(1, "#### state 3 ####\n");
    }

    mypid = getpid();
    printf(1, "PID %d is finished\n", mypid);

    exit();
}