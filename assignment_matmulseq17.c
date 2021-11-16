/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define NUM_THREADS 4
#define THREAD_ROWS_COUNT (SIZE / NUM_THREADS)

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
    int i[THREAD_ROWS_COUNT];
} thread_args;

void*
matmul_seq(void* args)
{
    thread_args* pargs = (thread_args*) args;
    int *rows = pargs->i;

    int i, j, k, r;
    for(i = 0; i < THREAD_ROWS_COUNT; i++){
        r = rows[i];
        for (j = 0; j < SIZE; j++) {
            c[r][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[r][j] = c[r][j] + a[r][k] * b[k][j];
        }
    }
}

void* matmul_init(void* args){
    
    thread_args* pargs = (thread_args*) args;
    int *rows = pargs->i;

    int i, j,r ;
    for(i = 0; i < THREAD_ROWS_COUNT; i++){
        r = rows[i];
        for (j = 0; j < SIZE; j++) {
	        a[r][j] = 1.0;
	        b[r][j] = 1.0;
        }
    }
}

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

int
main(int argc, char **argv)
{
    //init_matrix();

    pthread_t *threads;
    thread_args *args;
    threads = malloc(NUM_THREADS * sizeof(pthread_t));
    args = malloc(NUM_THREADS * sizeof(thread_args));

    for(int t = 0; t < NUM_THREADS; t++){
        for(int i = 0; i < THREAD_ROWS_COUNT; i++){
            args[t].i[i] = t * THREAD_ROWS_COUNT + i;
        }

        pthread_create(&threads[t], // our handle for the child
            NULL, // attributes of the child
            matmul_init, // the function it should run
            (void*)&args[t]); // args to that function
    }
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    for(int t = 0; t < NUM_THREADS; t++){
        for(int i = 0; i < THREAD_ROWS_COUNT; i++){
            args[t].i[i] = t * THREAD_ROWS_COUNT + i;
        }

        pthread_create(&threads[t], // our handle for the child
            NULL, // attributes of the child
            matmul_seq, // the function it should run
            (void*)&args[t]); // args to that function
    }

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    //matmul_seq_org();
    //print_matrix();
}
