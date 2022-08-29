
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

	int semid = semget(SEM_ALL_KEY, 3, IPC_CREAT | 0660);
													  
	semctl(semid, SEM_EMPTY, SETVAL, 3);
	semctl(semid, SEM_FULL, SETVAL, 0);
	semctl(semid, SEM_MUTEX, SETVAL, 1);

	int shmid = shmget(KEY, sizeof(int) * 4, IPC_CREAT | IPC_EXCL | 0666);
	if (shmid<0)
	{
		perror("create shm error.");
		return 0;
	}
	int *buf = (int *)shmat(shmid, 0, 0);
	for (int i = 0; i < 4; i++)
		buf[i] = 0;

	if (fork() == 0)
		execl("producer.out", 0);
	if (fork() == 0)
		execl("producer.out", 0);

	//生成消费者进程
	if (fork() == 0)
		execl("consumer.out", 0);
	if (fork() == 0)
		execl("consumer.out", 0);
	if (fork() == 0)
		execl("consumer.out", 0);

	int stat, i;
	for (i = 0; i<5; i++)
		wait(&stat);

	shmctl(shmid, IPC_RMID, 0);
	shmctl(semid, IPC_RMID, 0);
	
	return 0;

}

