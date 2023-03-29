#pragma once

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
#include<Windows.h>
#include<thread>
#include<condition_variable>
#include<deque>
#include<mutex>
#include<algorithm>
#include<vector>
#define max_player 8 //number of maximum players that can play simultaneously in a lobby

using namespace std;
deque<SOCKET> valid_connections;//valid connections which are coming but not given a lobby yet
deque<pair<int,int>> free_lobby;//queue of free lobbies
vector<int> total_lobbies;//total lobbies that the server has either free or occupied
//(lobby number,no of players)
class user_credentials
{
public:
	string username;
	string password;
};
class Mutex
{
public:
	mutex m_valid;//mutex for valid_connections
	mutex m_lobby;
	condition_variable is_data;//to check if there is more data to process
	condition_variable is_lobby;
};
Mutex* m;

void listener()
{
	/*this function will listen to the incoming socket requests and only forwards them to the next section only if the requests are valid
	valid requests are those whose username and password is correct*/
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo* bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);

	SOCKET socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		cout << "\n socket not created=>" << GETSOCKETERRNO();
	}
	int option = 0;
	if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&option, sizeof(option)))//for accepting ipv6 as well
	{
		cout << "\n problem in setting the flag==>";
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
	fd_set master;//master set containing all the sockets
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	SOCKET max_socket = socket_listen;
	deque<pair<SOCKET,int>> temp_socket;//temporary holder of new incoming connections
	int count = 0;
	while (1)
	{
		count++;
		fd_set reads;
		FD_ZERO(&reads);
		reads = master;
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		select(max_socket + 1, &reads, 0, 0, &timeout);
		for (int i = 1; i <= max_socket; i++)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == socket_listen)
				{
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address);
					SOCKET sock = accept(socket_listen, (sockaddr*)&client_address, &client_len);
					temp_socket.push_back(pair<SOCKET,int>(sock,count));
					FD_SET(sock, &master);
				}
				else//asking for username and passowrd
				{
					user_credentials cred;
					int bytes = recv(i, (char*)&cred, sizeof(cred), 0);
					if (1)//put the condition if the current user is verified or not
					{
						unique_lock<mutex> lk(m->m_valid);
						valid_connections.push_back(i);
						m->is_data.notify_one();
					}
					else//if the user is not authenticated then tear down the socket connection and remove it from the set of master
					{
						CLOSESOCKET(i);
						FD_CLR(i, &master);
						
					}
					
				}
			}

		}
	}
}
bool comparator(pair<int, int>& one, pair<int, int>& two)
{
	if (one.second >= two.second)
	{
		return true;
	}
	return false;
	

}
void lobby_contact(vector<SOCKET> &sockets)//sockets are the socket connection to the game servers
{
	fd_set master;
	FD_ZERO(&master);
	SOCKET max_socket = 0;
	for (int i = 0; i < sockets.size(); i++)
	{
		FD_SET(sockets[i], &master);
		if (i > max_socket)
		{
			max_socket = i;
		}
	}
	
	while (1)
	{
		fd_set reads;
		FD_ZERO(&reads);
		master = reads;
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		select(max_socket + 1, &reads, 0, 0, &timeout);
		for (int i = 0; i < sockets.size(); i++)
		{
			if (FD_ISSET(i, &reads))
			{
				pair<int, int> data;
				int byes = recv(i, (char*)&data, sizeof(data), 0);
				if (data.second == 0)
				{
					for (int j = 0; j < free_lobby.size(); j++)
					{
						if (free_lobby[j].first == data.first)
						{
							unique_lock<mutex> lk(m->m_lobby);
							auto it = free_lobby.begin();
							advance(it, j);
							free_lobby.erase(it);
							break;
						}
					}
				}
				else if(data.second==1)
				{
					unique_lock<mutex> lk(m->m_lobby);
					free_lobby.push_back(pair<int, int>(data.first, 0));
				}
			}
		}
	}
}
void assign_lobby()//to assign the lobby to the incoming authenticated connections
{
	while (1)
	{
		unique_lock<mutex> lk(m->m_valid);
		m->is_data.wait(lk, [] {return !valid_connections.empty(); });
		//enter only when the there are connections asking for the lobby
		deque<SOCKET> player_queue;
		for (int i = 0; i < valid_connections.size(); i++)
		{
			player_queue.push_back(valid_connections[i]);
		}
		valid_connections.clear();
		lk.unlock();

		unique_lock<mutex> lk1(m->m_lobby);
		m->is_lobby.wait(lk, [] { return !free_lobby.empty(); });
		deque<pair<int, int>> free_lobby1;
		for (int i = 0; i < free_lobby.size(); i++)
		{
			free_lobby1.push_back(free_lobby[i]);
		}
		lk1.unlock();
		//now queue has the connections to whom we will provide the lobbies
		//assuming now we know the status of the lobbies and are stored in free_lobbies dequeue

		sort(free_lobby1.begin(), free_lobby1.end(), comparator);
		for (int i = 0; i < free_lobby1.size(); i++)
		{
			
			if (player_queue.size()  < max_player-free_lobby1[i].second)
			{
				//transfer all the sockets to that particular process
				//....
				
				free_lobby1[i].second -= player_queue.size();
				player_queue.clear();
				break;
				
			}
			else if(player_queue.size() == max_player-free_lobby1[i].second)//if the players are more than the required in that particular lobby
			{
				//transfer all the sockets to that particular process
				//....

				free_lobby1[i].second -= player_queue.size();
				player_queue.clear();

				auto it = free_lobby1.begin();
				advance(it, i);
				free_lobby1.erase(it);
				i--;
				break;
			}
			else
			{
				//transfer first "max_player-free_lobby[i].second" sockets to the process
				//,....
				//removing the first max_player-free_lobby[i].second sockets
				for (int i = 1; i <= (max_player - free_lobby1[i].second); i++)
				{
					player_queue.pop_front();
				}
				//remove the lobby from free lobbies queue
				auto it = free_lobby1.begin();
				advance(it, i);
				free_lobby1.erase(it);
				i--;
			}
		}

	}
}
int main()
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		cout << "\n failed to initialize";
	}
#endif // defined

}










