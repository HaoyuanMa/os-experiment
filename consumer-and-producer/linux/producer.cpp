
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define KEY 1235
#define SEM_ALL_KEY 1234
#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2

void P(int sem_id, int sem_num)
{

	struct sembuf xx;
	xx.sem_num = sem_num;
	xx.sem_op = -1;
	xx.sem_flg = 0;
	semop(sem_id, &xx, 1);

}

void V(int sem_id, int sem_num)
{

	struct sembuf xx;
	xx.sem_num = sem_num;
	xx.sem_op = 1;
	xx.sem_flg = 0;
	semop(sem_id, &xx, 1);

}

int *buffer;
int main()
{
	int sem_id = semget(SEM_ALL_KEY, 3, IPC_CREAT | 0660);
	int i, j;
	int shmid = shmget(KEY, sizeof(int) * 4, 0);
	int *buf = (int *)shmat(shmid, 0, 0);


	for (i = 0; i<6; i++)
	{

		P(sem_id, SEM_EMPTY);
		P(sem_id, SEM_MUTEX);

		buf[0]++;
		buf[buf[0]] = 1;

						
		struct timeval curtime;
		gettimeofday(&curtime, NULL);

		printf("producer put product at %ld:%ld.\n", curtime.tv_sec, curtime.tv_usec);
		printf(" Now the buffer is : ");
		for (j = 1; j <= 3; j++)
			printf("%4d", buf[j]);
		printf("\n");

		V(sem_id, SEM_MUTEX);
		V(sem_id, SEM_FULL);

							

		sleep(random() % 5);

	}


}