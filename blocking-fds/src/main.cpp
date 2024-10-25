#include <asm-generic/errno-base.h>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdio.h> //printf

int main(void)
{
	// capturar as flags do stdin
	//int flags = fcntl(0, F_GETFL, 0);

	//setar modo non-blocking para stdin
	// if (fcntl(0, F_SETFL, flags | O_NONBLOCK) == -1)
	// {
	// 	printf("Could not change stdin to non-block mode");
	// }

	char* buffer = (char*) malloc(10);

	int bytes_read = read(0, buffer, 2);
	//codigo abaixar vai rodar quando fd estiver no modo não-bloqueante
	if (bytes_read == -1)
	{
		printf("Value of errno: %d\n", errno);
		perror("Error message: ");
	}
	
	write(1, buffer, 2);
}
