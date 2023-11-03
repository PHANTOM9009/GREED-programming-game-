
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif


#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif


#include <stdio.h>
#include <string.h>
#include<iostream>
#include<chrono>
using namespace std;
class data
{
public:
    int arr[4000];
    data()
    {
        memset(arr, 0, sizeof(arr));
    }
};
int main() 
{
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* bind_addres;

	//convert port to string
	
	getaddrinfo(0,"8080", &hints, &bind_addres);
	SOCKET tcp_socket = socket(bind_addres->ai_family, bind_addres->ai_socktype, bind_addres->ai_protocol);
	if (!ISVALIDSOCKET(tcp_socket))
	{
		cout << "\n socket not created=>" << GETSOCKETERRNO();
	}
	int enableKeepAlive = 1;
	setsockopt(tcp_socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&enableKeepAlive, sizeof(enableKeepAlive));

	cout << "\n binding the socket==>";
	if (bind(tcp_socket, (const sockaddr*)bind_addres->ai_addr, (int)bind_addres->ai_addrlen))
	{
		cout << "\n failed to bind the socket==>" << GETSOCKETERRNO();
	}
	listen(tcp_socket, 10);
	int count = 0;
	SOCKET re=0;
	fd_set master;
	fd_set read;
	FD_ZERO(&master);
	FD_ZERO(&read);
	FD_SET(tcp_socket, &master);
	struct timeval timeout;
	timeout.tv_usec = 0;
	timeout.tv_sec = 0;
	int max_socket = tcp_socket;
	while (count != 2)
	{
		read = master;
		
		select(max_socket + 1, &read, 0, 0, &timeout);
		if (FD_ISSET(tcp_socket, &read))
		{
			sockaddr_storage client_address;
			socklen_t client_length;
			SOCKET client = accept(tcp_socket, (sockaddr*)&client_address, (int*)&client_length);
			count++;
			client = re;
			cout << "\n connected with the client,...";
		}
	}
	char buff[100];
	int bytes = recv(re, buff, sizeof(buff), 0);
	if (bytes < 1)
	{
		cout << "\n bytes are==>" << bytes << " error is==>" << GETSOCKETERRNO();
	}
	CLOSESOCKET(tcp_socket);
	while (1)
	{

	}
    return 0;
}
