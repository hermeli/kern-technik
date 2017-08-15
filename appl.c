#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

int main( int argc, char **argv )
{
	int fd_gpio;
	int value;
	struct timespec sleeptime;

	fd_gpio=open("/dev/fastgpio4",O_WRONLY);
	
	if (fd_gpio<0) {
		printf("kann GPIO nicht oeffnen\n");
		return -1;
	}
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec= 1000; // 1us
	while (1) {
		value = 1;
		write(fd_gpio,&value,sizeof(value));
		clock_nanosleep( CLOCK_MONOTONIC, 0, &sleeptime, NULL );
		value = 0;
		write(fd_gpio,&value,sizeof(value));
		clock_nanosleep( CLOCK_MONOTONIC, 0,&sleeptime, NULL );
	}
	return 0;
}
