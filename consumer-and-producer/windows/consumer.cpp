
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	HANDLE SEM_FULL;
	HANDLE SEM_EMPTY;
	HANDLE SEM_MUTEX;
	HANDLE Map;



	for (int t = 0; t < 4; t++)
	{
		SEM_EMPTY = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPCWSTR)"EMPTY");
		SEM_FULL = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPCWSTR)"FULL");
		SEM_MUTEX = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPCWSTR)"MUTEX");

		Map = OpenFileMapping(FILE_MAP_WRITE, FALSE, (LPCWSTR)"buffer");
		int *buf = (int*)MapViewOfFile(Map, FILE_MAP_WRITE, 0, 0, sizeof(int) * 4);

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

		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(Map);

		Sleep(rand() % 5000);

	}
}
