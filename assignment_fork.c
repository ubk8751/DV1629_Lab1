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
    pid1 = fork();

    //If pid1 > 0 then we're in the parent
    if (pid1 > 0) {
        pid2 = fork();
        
        // If pid2 > 0 then we're in the parent
        if(pid2 > 0){
          printf("Parent: Child processes: %d, %d\n", pid1, pid2);
          for (i = 0; i < niterations; ++i)
              printf("B = %d, ", i);
        } 
        else if(pid2 == 0) {
          printf("Child 2\n");
          for (i = 0; i < niterations; ++i)
              printf("C = %d, ", i);
        } else {
          printf("Unknown process\n");
        }
     }

     //If pid1 == 0, it's thread 1
     else if (pid1 == 0){
        printf("Child 1\n");
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
     } else {
       printf("Unknown process\n");
     }
    printf("\n");
}
