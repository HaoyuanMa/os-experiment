## 操作系统课程设计实验报告
**实验名称：生产者和消费者问题**


<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [操作系统课程设计实验报告](#操作系统课程设计实验报告)
  - [一、实验目的](#一-实验目的)
  - [二、实验内容](#二-实验内容)
  - [三、实验环境](#三-实验环境)
  - [四、程序设计与实现](#四-程序设计与实现)
    - [windows](#windows)
    - [linux](#linux)
  - [五、实验结果](#五-实验结果)
    - [window](#window)
    - [linux](#linux-1)

<!-- /code_chunk_output -->


 ---

### 一、实验目的
- 学习使用共享内存和信号量实现进程间的通信。
- 学习利用信号量实现进程间的互斥与同步。
- 通过模拟进一步理解生产者和消费者问题模型。
- 学习windows和Linux下共享内存和信号量的实现方法。

### 二、实验内容
分别在windows和linux下模拟实现下列内容：
- 一个大小为3的缓冲区，初始为空
- 2个生产者
    - 随机等待一段时间，往缓冲区添加数据，
    - 若缓冲区已满，等待消费者取走数据后再添加
    - 重复6次
- 3个消费者
    - 随机等待一段时间，从缓冲区读取数据
    - 若缓冲区为空，等待生产者添加数据后再读取
    - 重复4次

### 三、实验环境
- CPU：intel core i7
- 内存：8G ddr4
- windows系统：win 10
- Linux系统：Ubuntu 18.04.3
- 虚拟机平台：VMware 14
- Linux 内核：Linux 5.3.11
- 开发工具：Visual Studio 2017 社区版

### 四、程序设计与实现
#### windows
- 主进程：
    - 创建共享内存和信号量并初始化。
        ```C++
        HANDLE SEM_FULL;
	    HANDLE SEM_EMPTY;
        HANDLE SEM_MUTEX;
        HANDLE Map;

        SEM_FULL = CreateSemaphore(NULL, 0, 3, (LPCWSTR)"FULL");
        SEM_EMPTY = CreateSemaphore(NULL, 3, 3, (LPCWSTR)"EMPTY");
        SEM_MUTEX = CreateSemaphore(NULL, 1, 1, (LPCWSTR)"MUTEX");

        HANDLE CurrentProcess = GetCurrentProcess();
        Map = CreateFileMapping(CurrentProcess, NULL, PAGE_READWRITE, 0, sizeof(int) * 4, (LPCWSTR)"buffer");

	    int *buf = (int*)MapViewOfFile(Map,FILE_MAP_WRITE, 0, 0, sizeof(int) * 4); 
        for (int i = 0; i < 4; i++)
		buf[i] = 0;
        ```

    - 创建2个生产者进程。
    - 创建3个消费者进程。
    - 等待5个进程结束。
        ```C++
        t = CreateProcess(program, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &is[1], &ip[1]);
        t = CreateProcess(program, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &is[2], &ip[2]);

        MultiByteToWideChar(CP_ACP, 0, str2, strlen(str2) + 1, program, sizeof(program) / sizeof(program[0]));
        t = CreateProcess(program, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &is[3], &ip[3]);
        t = CreateProcess(program, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &is[4], &ip[4]);
        t = CreateProcess(program, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &is[5], &ip[5]);

        for (int i = 1; i<=5; i++)
            WaitForSingleObject(ip[i].hProcess, INFINITE);
        ```

    - 关闭相关句柄
        ```C++
        for (int i = 1; i <= 5; i++)
		    CloseHandle(ip[i].hProcess);

        CloseHandle(SEM_MUTEX);
        CloseHandle(SEM_EMPTY);
        CloseHandle(SEM_FULL);
        CloseHandle(Map);
        ```
		
- 生产者进程：
    - 打开信号量集并映射共享内存。
        ```C++
        SEM_EMPTY = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPCWSTR)"EMPTY");
		SEM_FULL = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPCWSTR)"FULL");
		SEM_MUTEX = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPCWSTR)"MUTEX");

		Map = OpenFileMapping(FILE_MAP_WRITE, FALSE, (LPCWSTR)"buffer");
		int *buf = (int*)MapViewOfFile(Map, FILE_MAP_WRITE, 0, 0, sizeof(int) * 4);
        ```

    - P(empty) 	P(mutex)
    - 向缓存去放入产品并输出时间。
    - V(mutex)		V(full)
        ```C++
        WaitForSingleObject(SEM_EMPTY, INFINITE);
		WaitForSingleObject(SEM_MUTEX, INFINITE);

		buf[0]++;
		buf[buf[0]] = 1;

		SYSTEMTIME curtime;
		GetSystemTime(&curtime);


		printf("producer put product at %02d:%02d:%02d:%03d.\n ", curtime.wHour, curtime.wMinute, curtime.wSecond, curtime.wMilliseconds);
		printf("Now the buffer is : ");
		for (int j = 1; j <= 3; j++)
			printf("%4d", buf[j]);
		printf("\n");

		ReleaseSemaphore(SEM_MUTEX, 1, NULL);
		ReleaseSemaphore(SEM_FULL, 1, NULL);
        ```

    - 关闭相关句柄。
    - 利用Sleep等待随机事件。
    - 将上述过程重复6次。
		
- 消费者进程：
    - 与生产者类似，P/V操作处顺序不同，重复四次。
        ```C++
        WaitForSingleObject(SEM_FULL, INFINITE);
		WaitForSingleObject(SEM_MUTEX, INFINITE);

		buf[buf[0]] = 0;
		buf[0]--;
		
		SYSTEMTIME curtime;
		GetSystemTime(&curtime);

		printf("consumer get product at %02d:%02d:%02d:%03d.\n ", curtime.wHour, curtime.wMinute, curtime.wSecond, curtime.wMilliseconds);
		printf("Now the buffer is : ");
		for (int j = 1; j <= 3; j++)
			printf("%4d", buf[j]);
		printf("\n");

		ReleaseSemaphore(SEM_MUTEX, 1, NULL);
		ReleaseSemaphore(SEM_EMPTY, 1, NULL);
        ```

#### linux
- 主进程：
    - 定义封装P，V操作。
        ```C++
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
        ```

    - 申请共享内存和信号量并初始化。
        ```C++
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
        ```

    - 创建2个生产者进程。
    - 创建3个消费者进程。
    - 等待5个子进程结束。
        ```C++
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
        ```

    - 关闭相关标识符
        ```C++
        shmctl(shmid, IPC_RMID, 0);
	    shmctl(semid, IPC_RMID, 0);
        ```

- 生产者进程：
    - 打开信号量集并映射共享内存。
        ```C++
        int sem_id = semget(SEM_ALL_KEY, 3, IPC_CREAT | 0660);
        int i, j;
        int shmid = shmget(KEY, sizeof(int) * 4, 0);
        int *buf = (int *)shmat(shmid, 0, 0);
        ```

    - P(empty)		P(mutex)
    - 向缓存去放入产品
    - 输出时间
    - V(mutex)		V(full)
        ```C++
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
        ```
 
    - 等待随机时间，并重复上述过程6次
		
- 消费者进程：
	- 与生产者类似，P/V操作处顺序不同，重复四次。
        ```C++
        P(sem_id, SEM_FULL);
		P(sem_id, SEM_MUTEX);


		buf[buf[0]] = 0;
		buf[0]--;   

		struct timeval curtime;
		gettimeofday(&curtime, NULL);

		printf("consumer get product at %ld:%ld.\n", curtime.tv_sec, curtime.tv_usec);
		printf(" Now the buffer is : ");
		for (j = 1; j <= 3; j++)
			printf("%4d", buf[j]);
		printf("\n");

		V(sem_id, SEM_MUTEX);
		V(sem_id, SEM_EMPTY);

		sleep(random() % 5);
        ```
### 五、实验结果
#### window
![win][pic-0]

#### linux
![linux][pic-1]

---

[pic-0]: https://vkceyugu.cdn.bspapp.com/VKCEYUGU-1682933a-c290-4a19-a517-c44d14df20fc/ac216f59-2b27-46be-958c-9acb40c3679f.png
[pic-1]: https://vkceyugu.cdn.bspapp.com/VKCEYUGU-1682933a-c290-4a19-a517-c44d14df20fc/2e62dae5-a948-4750-a263-c45830d02e9e.png



		


