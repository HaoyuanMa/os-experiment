#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	int pid, endcode;
	timeval start, stop;

	char *path = getenv("PATH");
	char *PATH[] = {path, "DISPLAY=:0.0", NULL };

	gettimeofday(&start, NULL);

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
	else
	{
		printf("fork failed!\n");
	}

    return 0;
}