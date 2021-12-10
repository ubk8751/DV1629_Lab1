#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // Creates 2 child variables to store the IDs
    pid_t pid1;
    pid_t pid2;
    unsigned i;
    unsigned niterations = 1000;

    // fork twice and save the child IDs
    int pids[2];
    for(int i = 0; i < 2; i++)
      pids[i] = fork();

    //If both pids are greater than 0, the thread is the parent
    if (pids[0] > 0 && pids[1] > 0) {
        //if pid2 == 0, it's thread 2
        printf("Parent\n");
        printf("Child 1 id: %i, Child 2 id: %i\n", pids[0], pids[1]);
        for (i = 0; i < niterations; ++i)
            printf("B = %d, ", i);
     }

     //If pid1 == 0, it's thread 1
     else if(pids[0] == 0 && pids[1] > 0){
        printf("Child 1\n");
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
     }
    else if(pids[0] > 0 && pids[1] == 0){
        printf("Child 2\n");
        for (i = 0; i < niterations; ++i)
          printf("C = %d, ", i);
    }

    printf("\n");
}
