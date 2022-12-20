/*
* job of the connector sever will be issue unique id to all the clients so that they can show their ID's when they are
* connected with the real game server
* 
*/
#include<SFML/Graphics.hpp>


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
#include<iostream>
#include<string>
#include<vector>
using namespace std;

enum class errorcode
{
	TIME_EXCEEDED = 0, LOBBY_FULL
};
int busy = 0;
int current_players = 0;
int max_players = 7;

fd_set set_game()//to accept the connections and make the lobby
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		cout << "\n failed to initialize";
	}
#endif // defined
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	hints.ai_flags = AI_PASSIVE;


	struct addrinfo* bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);
	SOCKET socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		cout << "\n socket not created==>" << GETSOCKETERRNO();
	}

	int option = 0;
	if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&option, sizeof(option)))//for accepting ipv6 as well
	{
		cout << "\n problem in setting the flag==>";
	}
	int yes = 1;
	if (setsockopt(socket_listen, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes)) < 0) //disabling nagle's algorithm for speed in sending the data
	{
		fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO());
	}


	cout << "\n binding the socket==>";
	if (bind(socket_listen, (const sockaddr*)bind_address->ai_addr, (int)bind_address->ai_addrlen))
	{
		cout << "\n failed to bind the socket==>" << GETSOCKETERRNO();
	}

	if (listen(socket_listen, 20) < 0)
	{
		cout << "\n socket failed";
	}
	freeaddrinfo(bind_address);
	sf::Time time;
	sf::Clock clock;
	double time_elapsed;
	double time_elapsed_limit = 5;//in minutes
	fd_set master;
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	SOCKET max_socket = socket_listen;
	errorcode code;
	while (1)
	{
		time = clock.restart();
		time_elapsed += time.asSeconds();
		if (time_elapsed / 60 >= time_elapsed_limit)//lobby time has exceeded break all the current connections
		{
			for (int i = 1; i <= max_socket; i++)
			{
				if (FD_ISSET(&master, i))
				{
					CLOSESOCKET(i);
				}
			}
			code = errorcode::TIME_EXCEEDED;
			break;

		}
		if (current_players == max_players)//current players are equal to max players that can play at once
		{
			code = errorcode::LOBBY_FULL;
			break;
		}
	}
}