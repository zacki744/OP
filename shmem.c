#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200
#define SIZE_OF_BUFFER 10


int main(int argc, char **argv)
{
	struct shm_struct {
		int buffer[10];
		unsigned empty;
	};
	srand(time(NULL));
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
	shmp->empty = 0;
	pid = fork();
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;
			//if(shmp->empty == 10)
				//while (shmp->empty > 0); /* busy wait until the buffer is empty */
			printf("Sending %d\n", var1); fflush(stdout);
			shmp->buffer[(shmp->empty ++) % SIZE_OF_BUFFER] = var1;
			int r = rand() % (5 - 1 + 1) + 1;
			sleep(r/10);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
			//if (shmp->empty == 0)
				//while (shmp->empty < 10); /* busy wait until there is something */
			//
			var2 = shmp->buffer[(shmp->empty ++) % SIZE_OF_BUFFER];
			printf("Received %d\n", var2); fflush(stdout);
			//designates a random number
			int r = rand() % (20 - 2 + 1) + 2;
			//sleap for a time
			sleep(r/10);
			shmdt(addr);
			shmctl(shmid, IPC_RMID, shm_buf);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);	
	}
	return 0;
}
