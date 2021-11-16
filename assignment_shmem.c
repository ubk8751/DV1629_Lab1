#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

// /home/saxon/students/20192/sajs19/studentfiles/lab1_code_students_v1.1
// gcc -O2 -o out shmem.c && chmod +x out && ./out

int main(int argc, char **argv)
{
	struct shm_struct {
		int buffer[10];
		int buf_write_pos;
		int buf_read_pos;
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

	for(int i = 0; i < 10; i++)
		shmp->buffer[i] = -1;

	pid = fork();
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;
			while(shmp->buffer[shmp->buf_write_pos] != -1);
			shmp->buffer[shmp->buf_write_pos] = var1;
			shmp->buf_write_pos++;

			if(shmp->buf_write_pos > 9)
				shmp->buf_write_pos = 0;

			printf("Sending %d\n", var1); fflush(stdout);

			// This is REALLY bad because it means the program will sleep for random amount of time
			// which in turn means that it will basically print stuff at random intervalls.
			// If the "sender" does this as well, it means that the buffer might not be filled up until the
			// point where we are reading. This also means that there is a chance the "receiver" sleeps so much that the
			// sender manages to loop around and you lose a lot of data.

			// There is also no specific check to whether or not the sender and receiver are trying to access the data at the same time. Which could cause problems.
			int slp = (rand() % 100 + 100) * 1000;
			usleep(slp);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */

			while(shmp->buffer[shmp->buf_read_pos] == -1);
			var2 = shmp->buffer[shmp->buf_read_pos];
			shmp->buffer[shmp->buf_read_pos] = -1;
			shmp->buf_read_pos++;

			if(shmp->buf_read_pos > 9)
				shmp->buf_read_pos = 0;

			printf("Received %d\n", var2); fflush(stdout);

			// This is REALLY bad because it means the program will sleep for random amount of time
			// which in turn means that it will basically print stuff at random intervalls.
			// If the "sender" does this as well, it means that the buffer might not be filled up until the
			// point where we are reading. This also means that there is a chance the "receiver" sleeps so much that the
			// sender manages to loop around and you lose a lot of data.
			int slp = (rand() % 100 + 100) * 1000;
			usleep(slp);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
}
