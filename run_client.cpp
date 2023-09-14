
/*
 * MIT License
 *
 * Copyright (c) 2018 Lewis Van Winkle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
int main() {

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* bind_address;
    cout << "\n enter the ip address=>";
    string ip;
    cin >> ip;
    getaddrinfo(ip.c_str(), "8080", &hints, &bind_address);


    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
        bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    connect(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen);


    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    char read[100] = "hello";
	int bytes = sendto(socket_listen, read, sizeof(read), 0, bind_address->ai_addr, bind_address->ai_addrlen);
    if (bytes < 1)
    {
		cout << "\n client cannot send bytes to the server==>" << GETSOCKETERRNO();
    }
    else
    {
        cout << "\n sent hello to the server";
    }
    
    int n;
    
    const int MAX_SIZE = 1000;
    int found;
    while (1)
    {
        ::data ob;
        int recv_bytes = 0;
        char comp[sizeof(ob)];
        int checker;
        if (found == 0)
        {
            recv(socket_listen, (char*)&checker, sizeof(checker), 0);
        }
        if(checker==1 || found==1)
        {
            while (recv_bytes < sizeof(ob))
            {
                char buffer[1000];
                found = 0;
                bytes = recv(socket_listen, (char*)&buffer, sizeof(buffer), 0);
                if (bytes < 1)
                {
                    cout << "\n cannot recv the bytes==>" << GETSOCKETERRNO();
                }
                else if (bytes == 4)
                {
                    int check;
                    memcpy(&check, buffer, sizeof(int));
                    if (check == 1)
                    {
                        cout << "\n problem occured breaking...";
                        found = 1;
                        break;
                    }
                }
                else if (bytes == 1000)
                {
                    if (bytes == 4)
                    {
                        int check;
                        memcpy(&check, buffer, sizeof(int));
                        if (check == 101)
                        {
                            //new packet is starting so break out of the loop
                        }
                    }
                    memcpy(comp + recv_bytes, buffer, bytes);
                    recv_bytes += bytes;
                    //cout << "\n recved bytes from the server=>" << bytes << " " << ob.arr[100];
                    auto now = std::chrono::system_clock::now();
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                    auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
                    auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
                    auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

                    //cout << "\n recved data from the server at the time==> " <<
                    //hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
                }
               
            }
            memcpy(&ob, comp, sizeof(ob));
            cout << "\n value of the packet==>" << ob.arr[0] << " " << ob.arr[3999];
        }
    }

    
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
