/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
	        /* Simple initialization, which enables us to easy check
	         * the correct answer. Each element in c will have the same
	         * value as SIZE after the matmul operation.
	         */
	        a[i][j] = 1.0;
	        b[i][j] = 1.0;
        }
}
static void
matmul_seq_org()
{
    int i, j, k;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

typedef struct {
    int i;
} thread_args;

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
	        printf(" %7.2f", c[i][j]);
	    printf("\n");
    }
}


void*
matmul_seq(void* args)
{
    thread_args* pargs = (thread_args*) args;
    int row = pargs->i;

    int j, k;
    for (j = 0; j < SIZE; j++) {
        c[row][j] = 0.0;
        for (k = 0; k < SIZE; k++)
            c[row][j] = c[row][j] + a[row][k] * b[k][j];
    }
}

int
main(int argc, char **argv)
{
    init_matrix();

    pthread_t *threads;
    thread_args *args;
    threads = malloc(SIZE * sizeof(pthread_t));
    args = malloc(SIZE * sizeof(thread_args));

    for(int t = 0; t < SIZE; t++){
        args[t].i = t;

        pthread_create(&threads[t], // our handle for the child
            NULL, // attributes of the child
            matmul_seq, // the function it should run
            (void*)&args[t]); // args to that function
    }

    for(int i = 0; i < SIZE; i++){
        pthread_join(threads[i], NULL);
    }
    //matmul_seq_org();
    //print_matrix();
}
