## 操作系统课程设计实验报告
**实验名称：进程控制**

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
- 熟悉不同系统下如何创建进程。
- 学习在编程中实现进程控制。
- 学习不同系统下的进程控制。

### 二、实验内容
设计并实现Unix的“time”命令。“mytime”命令通过命令行参数接受要运行的程序，创建一个独立的进程来运行该程序，并记录程序运行的时间。

### 三、实验环境
- CPU：intel core i7
- 内存：8G ddr4
- windows系统：win 10
- linux系统：Ubuntu 18.04.3
- 虚拟机平台：VMware 14
- linux 内核：linux 5.3.11
- 开发工具：Visual Studio 2017 社区版

### 四、程序设计与实现
#### windows
- 分离出目标程序路径，将目标程序的命令行参数组合到同一字符串。并使用MultiByteToWideChar()将其参数转换成宽字符类型。
    ```C++
    char arg[100];
	strcpy(arg, argv[1]);
	for (int i = 2; i < argc; i++)
	{
		int len = strlen(arg);
		arg[len] = ' ';
		arg[len + 1] = '\0';
		strcat(arg, argv[i]);
	}
	
	WCHAR cmdline[100];
	memset(cmdline, 0, sizeof(cmdline));
	MultiByteToWideChar(CP_ACP, 0, arg, strlen(arg) + 1, cmdline, sizeof(cmdline) / sizeof(cmdline[0]));
    ```

- 使用GetSystemTime()的到当前系统时间。
    ```C++
    SYSTEMTIME start;
	GetSystemTime(&start);
    ```
 
- 尝试使用CreateProcess(program, cmdline,…)创建进程，其中program是新程序名，cmdline是其命令行参数。
- 若3）失败说明当前目录下未找到相应程序，则新程序为系统程序，使用CreateProcess(NULL, cmdline,…)继续创建。
    ```C++
    if (CreateProcess(program, cmdline, NULL, NULL, 0, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&stStartUpInfo, &stProcessInfo) == 0)
	{
		if (CreateProcess(NULL, cmdline, NULL, NULL, 0, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&stStartUpInfo, &stProcessInfo) == 0)
		{
			printf("Create failed with code %d !\n", GetLastError());
			return 0;
		}
	}
    ```

- 使用WaitForSingleObject()等待新进程运行完毕。
- 再次使用GetSystemTime()的到当前系统时间。
    ```C++
    WaitForSingleObject(stProcessInfo.hProcess, INFINITE);

	SYSTEMTIME stop;
	GetSystemTime(&stop);
    ```
 
- 计算所用时间并格式化输出。
    ```C++
    int stime = (start.wHour * 60 * 60 + start.wMinute * 60 + start.wSecond) * 1000 + start.wMilliseconds;
	int etime = (stop.wHour * 60 * 60 + stop.wMinute * 60 + stop.wSecond) * 1000 + stop.wMilliseconds;
	int time = etime - stime;

	int ms = time % 1000;
	time /= 1000;
	int s = time % 60;
	time /= 60;
	int m = time % 60;
	int h = time /= 60;

	printf("\n%02d h %0d m %02d s %02d ms %02d um\n", h, m, s, ms, 0);
    ```

#### linux
- 得到环境变量，使用gettimeofday()得到当前时间。
    ```C++
    char *path = getenv("PATH");
	char *PATH[] = {path, "DISPLAY=:0.0", NULL };

	gettimeofday(&start, NULL);
    ```
 
- 使用fork()创建子进程。
- 在父进程使用wait()等待子进程结束。并再次使用gettimeofday()得到结束时间，计算所用时间并格式化输出。
    ```C++
    if ((pid = fork()) > 0)
	{
		wait(&endcode);

		gettimeofday(&stop, NULL);

		int time = (stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
		int us = time % 1000;
		time /= 1000;
		int ms = time % 1000;
		time /= 1000;
		int s = time % 60;
		time /= 60;
		int m = time % 60;
		int h = time / 60;
		printf("\n%02d h %0d m %02d s %02d ms %02d um\n", h, m, s, ms, us);
	}
    ```
 
- 在子进程中尝试使用execvpe()运行新程序，若3）失败，说明新程序不是系统程序，使用execve()运行。
    ```C++
    else if (pid == 0)
	{
		char **argvc = &argv[1];
		argvc[argc] = NULL;
		
		if ((execvpe(argv[1], argvc, PATH)) == -1)
		{
			execv(argv[1], argvc);
		}

		exit(0);
	}
    ```

### 五、实验结果
#### window
![win][pic-0]
![win][pic-1]

#### linux
![linux][pic-2]
![linux][pic-3]

---

[pic-0]: https://images.wait4echo.love/Works/os-experiment/ose-time-0.png
[pic-1]: https://images.wait4echo.love/Works/os-experiment/ose-time-1.png
[pic-2]: https://images.wait4echo.love/Works/os-experiment/ose-time-2.png
[pic-3]: https://images.wait4echo.love/Works/os-experiment/ose-time-3.png



		


