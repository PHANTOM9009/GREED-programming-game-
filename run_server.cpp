#pragma once
#include<SFML/Graphics.hpp>
#include<conio.h>

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
#define GETSOCKETERRNO() WSAGetLastError()


#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif
#include<ctime>
#include "online_lib2.hpp"
#include "online_lib2.cpp"

int main(int argc,char *argv [])
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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* bind_address;
	//convert port to string
	
	getaddrinfo(0,"8085", &hints, &bind_address);
	
	printf("Creating socket...\n");

	SOCKET socket_peer = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	
	if (!ISVALIDSOCKET(socket_peer))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());

	}
	if (::bind(socket_peer,
		bind_address->ai_addr, bind_address->ai_addrlen)) {
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());

	}
    
    int n = 0;
	char msg[100];
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	int b = recvfrom(socket_peer, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client_address, &client_len);
	cout << "\n message from client==>" << msg;
	//recving message from the client terminal to whom the data has to be sent
	/*
	memset(msg, 0, sizeof(msg));
	struct sockaddr_storage client_address1;
	socklen_t client_len1 = sizeof(client_address1);
	b = recvfrom(socket_peer, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client_address1, &client_len1);
	if (b < 1)
	{
		cout << "\n cannot recv bytes from the client terminal..";

	}
	cout << "\n message from client terminal=>" << msg;
	*/
    while (1)
    {
		recv_data data;
		struct sockaddr_storage client_address;
		socklen_t client_len = sizeof(client_address);
		int bytes = recvfrom(socket_peer, (char*)&data, sizeof(data), 0, (sockaddr*)&client_address, &client_len);
		if (bytes < 1)
		{
			cout << "\n cannot recv the bytes from the client..";
		}
		cout << "\n recved bytes from the client==>" << data.packet_id;
		
		//sending the data to the client terminal
		/*
		bytes = sendto(socket_peer, (char*)&data, sizeof(data), 0, (sockaddr*)&client_address1, client_len1);
		if (bytes < 1)
		{
			cout << "\n cannot send bytes to the client==>" << GETSOCKETERRNO();
				
		}
		*/
    }
    

    CLOSESOCKET(socket_peer);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
