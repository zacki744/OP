#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid;
    pid_t pid_2;

    unsigned i;
    unsigned niterations = 100;
    pid = fork();
    pid_2 = fork();
    if (pid == 0) {
        for (i = 0; i < niterations; ++i)
        {
            printf("A = %d, \n", i);
        }

    } else if (pid_2 > 0) {
        printf("process = %d, \n", pid_2);
        for (i = 0; i < niterations; ++i)
        {
            printf("B = %d, \n", i);
        } 

     } else {
        printf("process = %d, \n", pid);
        for (i = 0; i < niterations; ++i)
        {
            printf("C = %d, \n", i);
        }    
    } 
}
