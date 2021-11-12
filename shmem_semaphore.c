#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> /* For O_* constants */
#include <errno.h>

// /home/saxon/students/20192/sajs19/studentfiles/lab1_code_students_v1.1
// gcc -O2 -o out shmem.c && chmod +x out && ./out

const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";

int main(int argc, char **argv)
{
	sem_unlink(semName1);
	sem_unlink(semName2);
	sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 10);
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0);

	int i, status;
	struct shm_struct {
		int buffer[10];
		int buf_write_pos;
		int buf_read_pos;
		int has_initialised;
	};

	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
	shmp->buf_write_pos = 0;
	shmp->buf_read_pos = 0;

	pid = fork();
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
      sem_wait(sem_id1);
			var1++;
			shmp->buffer[shmp->buf_write_pos] = var1;
			shmp->buf_write_pos++;

			if(shmp->buf_write_pos > 9)
				shmp->buf_write_pos = 0;

			printf("Sending %d\n", var1); fflush(stdout);

      sem_post(sem_id2);

			int slp = (rand() % 100 + 100) * 1000;
			usleep(slp);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);

		sem_close(sem_id1);
		sem_close(sem_id2);

		sem_unlink(semName1);
		sem_unlink(semName2);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
      sem_wait(sem_id2);

			var2 = shmp->buffer[shmp->buf_read_pos];
			shmp->buffer[shmp->buf_read_pos] = -1;
			shmp->buf_read_pos++;

			if(shmp->buf_read_pos > 9)
				shmp->buf_read_pos = 0;

			printf("Received %d\n", var2); fflush(stdout);

      sem_post(sem_id1);
			int slp = (rand() % 100 + 100) * 1000;
			usleep(slp);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);

		sem_close(sem_id1);
		sem_close(sem_id2);
	}
}
