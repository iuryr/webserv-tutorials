#include <cstdio>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>

#define MAX_EVENTS 10

int main(void)
{

	int epoll_fd = epoll_create1(0);

	struct epoll_event event_of_interest;

	//A ausência de EPOLLET faz epoll_wait retornar sempre que o fd ainda tenha alguma coisa
	//para ser lida do buffer
	event_of_interest.events = EPOLLIN;

	//EPOLLET faz epoll_wait blockar até um novo evento acontecer no fd de interesse
	// event_of_interest.events = EPOLLIN | EPOLLET;
	event_of_interest.data.fd = STDIN_FILENO;

	struct epoll_event event_list[MAX_EVENTS];

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event_of_interest);

	char *read_buffer = (char*) malloc(10);
	int fds_ready;

	while(42)
	{
		fds_ready = epoll_wait(epoll_fd, event_list, MAX_EVENTS, -1);
		printf("fds ready for IO: %d\n", fds_ready);

		for (int i = 0; i < fds_ready; i++)
		{
			read(event_list[i].data.fd, read_buffer, 2);
			write(1, read_buffer, 2);
			write(1, "\n", 1);
		}
	}
}
