#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char** argv){
    int pid;
    int mypid;

    setnice(1, 10);

    setnice(getpid(), 2);

    pid = fork();

    if(pid == 0){
        printf(1, "#### state 2 ####\n");
    }else{
        printf(1, "#### state 1 ####\n");

        setnice(pid, 10);
        wait();
        printf(1, "####state 3 ####\n");
    }

    mypid = getpid();
    printf(1, "PID %d is finished\n", mypid);

    exit();
}
