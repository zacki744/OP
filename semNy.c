#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> /* For O_* constants */

const char *semName1 = "my_sema1";
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200
#define SIZE_OF_BUFFER 10


int main(int argc, char **argv)
{
	struct shm_struct {
		int buffer[10];
		unsigned int empty;
        sem_t *ocopied;
	};
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
    sem_init(shmp->ocopied, 0, 1);

	shmp->empty = 0;
	pid = fork();
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
            sem_wait(&shmp->ocopied);
			var1++;
			printf("Sending %d\n", var1); fflush(stdout);
			shmp->buffer[(shmp->empty ++) % SIZE_OF_BUFFER] = var1;
            sem_post(&shmp->ocopied);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
            sem_wait(shmp->ocopied);
			var2 = shmp->buffer[(shmp->empty ++) % SIZE_OF_BUFFER];
			printf("Received %d\n", var2); fflush(stdout);
            sem_post(shmp->ocopied);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);	
	}
	return 0;
}
