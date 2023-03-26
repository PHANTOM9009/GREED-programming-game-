//to test the socket transfer
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
using namespace std;
int main()
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
	int res = 0;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* server_add;
	char buff[1000];
	getaddrinfo("127.0.0.1", "8080", &hints, &server_add);
	getnameinfo(server_add->ai_addr, server_add->ai_addrlen, buff, sizeof(buff), 0, 0, NI_NUMERICHOST);
	cout << "\n the server address is==>" << buff;
	cout << endl;
	SOCKET peer_socket = socket(server_add->ai_family, server_add->ai_socktype, server_add->ai_protocol);
	cout << "\n socket number of the user is==>" << peer_socket;
	if (!ISVALIDSOCKET(peer_socket))
	{
		cout << "\n socket not created==>" << GETSOCKETERRNO();
	}
	/**/
	while (connect(peer_socket, server_add->ai_addr, server_add->ai_addrlen))
	{
		cout << "\n problem in connecting";

	}
	cout << "\n client connected with server";
	int yes = 1;
	if (setsockopt(peer_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes)) < 0) //disabling nagle's algorithm for speed in sending the data
	{
		fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO());
	}
	//cout << "\n connected with the server..";
	freeaddrinfo(server_add);
	char buff2[1000];
	int bytes1=recv(peer_socket,buff2,sizeof(buff2),0);
	cout<<"\n bytes recv=>"<<bytes1;
	cout<<"\ndata=>"<<buff2;
	while (1)
	{

	}

}
