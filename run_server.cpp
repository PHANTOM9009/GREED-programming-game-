
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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* bind_address;
	//convert port to string
	
	getaddrinfo(0,"8080", &hints, &bind_address);
	
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
    struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	char read[1024];
    int bytes = recvfrom(socket_peer, read, 1024, 0, (sockaddr*)&client_address, &client_len);
    if (bytes < 1)
    {
        cout << "\n cannot recv the bytes from the client==>" << GETSOCKETERRNO();
    }
    else
    {
        cout << "\n the message recved is==>" << read;
    }
    int n = 0;
   
    const int MAX_LENGTH = 1000;
    
    while (1)
    {
        ::data ob;
        ob.arr[0] = n;
        ob.arr[3999] = n-1;
        char buffer[sizeof(ob)];
        memcpy(buffer, &ob, sizeof(ob));
       
        int sent_bytes = 0;
        //sending that starting a new packet
        int sending_new = 1;
        sendto(socket_peer,(char*)&sending_new,sizeof(sending_new), 0, (sockaddr*)&client_address, client_len);
        while (sent_bytes < sizeof(ob))
        {
            
			int bytesToSend = min(MAX_LENGTH, sizeof(ob) - sent_bytes);
            bytes = sendto(socket_peer, buffer+sent_bytes, bytesToSend, 0, (sockaddr*)&client_address, client_len);
            if (bytes < 1)
            {
                cout << "\n cannot send bytes to the client==>" << GETSOCKETERRNO();
            }
            else
            {
                sent_bytes += MAX_LENGTH;
                cout << "\n sent=>" << bytes;
                auto now = std::chrono::system_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
                auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
                auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

               // cout << "\n sent data to the client at==> " <<
                 //   hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
            }
        }
        cout << "\n sent one packet-------------------------------------------------------------";
        Sleep(100);
        
        
        n++;
    }
    

    CLOSESOCKET(socket_peer);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
