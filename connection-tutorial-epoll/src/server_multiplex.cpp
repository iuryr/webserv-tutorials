#include "qolMacros.hpp"
#include <cstdlib> //exit
#include <cstring> //memset 
#include <fstream> //close
#include <sys/socket.h> //socket 
#include <sys/types.h>
#include <sys/epoll.h> //epoll API
#include <netdb.h>
#include <unistd.h>


int main(void)
{
	//create socket
	//AF_INET -> family of protocols. IPv4
	//SOCK_STREAM -> forma de comunicação. TCP
	//0 -> protocolo específico da família. A maioria das família só implementa um, daí passamos 0. É o caso do IPv4.
	int sock_fd;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	//always check if a system call was sucessfull or erroed
	if (sock_fd == -1)
	{
		exit(1);
	}
	println("Socket fd: " << sock_fd);

	//O struct sockaddr_in contém informações sobre endereços usados para comunicação via IP. O struct sockaddr é mais geral.
	//Declaramos o struct na stack e setamos todos seu elementos para zero com memset - para não correr o risco de pegar lixo de memória
	//Para ver o formato do struct sockaddr_in: man 7 ip
	struct sockaddr_in address; //versao especial de struct sockaddr
	std::memset(&address, 0, sizeof(sockaddr_in));
	//Vamos setar alguns elementos de address para valores que fazem sentido para nós
	// INADDR_ANY -> all available addresses (loopback aka localhost, public IP, etc)
	address.sin_addr.s_addr = INADDR_ANY; 
	//AF_INET -> protocol family
	address.sin_family = AF_INET; //protocol family
	//port -> uma porta é um número de 16 bits que identifica um destino específico dentro de um endereço
	// htons é só uma função que modifica o parâmetro para ele estar na 'network bit order', que é big endian
	address.sin_port = htons(8081);
	

	//Agora vamos associar um endereço + porta ao fd que geramos no início
	int bind_status;
	//Perceba que fazemos o cast the sockaddr_in para sockaddr. sockaddr é um struct mais geral para outros tipos de endereço
	bind_status = bind(sock_fd, (struct sockaddr*)&address, sizeof(address));
	if (bind_status == -1)
	{
		println("bind failed");
		exit(1);
	}
	println("Bind succeeded");

	//Agora que sock_fd está associado a um endereço + porta, vamos disponibilizá-lo para receber conexões
	int listen_status;
	listen_status = listen(sock_fd, 10);
	if (listen_status != 0)
	{
		println("Listen failed");
		exit(1);
	}

	//Creating epoll instance
	int epoll_fd;
	epoll_fd = epoll_create1(0);

	//Adding epoll_fd to epoll instance to monitor
	//First we need to populate a struct epoll_event to passa to the systemcall
	struct epoll_event ev;
	ev.data.fd = sock_fd;
	ev.events = EPOLLIN;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev);

	while(42)
	{
		
		int new_fd = accept(sock_fd, NULL, NULL);
		if (new_fd != -1)
		{
			println("Connection accepted");
			
			// Construct the HTTP response
			const char *response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: 13\r\n"
				"\r\n"
				"<h1>OIE</h1>";

			// Send the HTTP response to the client
			ssize_t bytes_sent = send(new_fd, response, std::strlen(response), 0);
			if (bytes_sent == -1)
			{
				println("Failed to send response");
			}
			else
			{
				println("Response sent successfully");
			}

			// Close the connection
			close(new_fd);
		}
	}
}

