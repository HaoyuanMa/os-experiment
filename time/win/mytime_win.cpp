#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <windows.h>


int main(int argc, char* argv[]) {

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(stStartUpInfo));
	stStartUpInfo.cb = sizeof(stStartUpInfo);
	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));

	WCHAR program[100];
	memset(program, 0, sizeof(program));
	MultiByteToWideChar(CP_ACP, 0, argv[1], strlen(argv[1]) + 1, program, sizeof(program) / sizeof(program[0]));

	/*printf("%s\n", argv[0]);
	wprintf(L"%ls\n", program);*/

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

	SYSTEMTIME start;
	GetSystemTime(&start);

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

	WaitForSingleObject(stProcessInfo.hProcess, INFINITE);

	SYSTEMTIME stop;
	GetSystemTime(&stop);

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

	return 0;
}
