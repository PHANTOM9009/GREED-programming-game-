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
using namespace std;
class transfer_socket
{
public:
    int len;
    WSAPROTOCOL_INFO protocolInfo[100];

};
std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
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
    cout << "\n my process id is=>" << GetCurrentProcessId();
    while(connect(peer_socket,server_add->ai_addr,server_add->ai_addrlen))
    {
        cout << "\n problem in connecting";
    }
    cout << "\n sending the id=>";
    int val = GetCurrentProcessId();
    int bytes = send(peer_socket, (char*)&val, sizeof(val), 0);

    // Create a socket using the protocol info
    int total = 0;

    fd_set master;
    FD_ZERO(&master);
    FD_SET(peer_socket, &master);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    SOCKET childSocket;
    while (total < 2)
    {

        transfer_socket ob;
        int byte = recv(peer_socket, (char*)&ob, sizeof(ob), 0);
        for (int i = 0; i < ob.len; i++)
        {
            childSocket = WSASocket(ob.protocolInfo[i].iAddressFamily, ob.protocolInfo[i].iSocketType, ob.protocolInfo[i].iProtocol, &ob.protocolInfo[i], 0, WSA_FLAG_OVERLAPPED);
            if (childSocket == INVALID_SOCKET)
            {
                std::cerr << "Failed to create child socket: " << WSAGetLastError() << std::endl;
                return 1;
            }
            char response[100] = "Hello from child process!\0";
            int sendResult = send(childSocket, (char*)&response, 100, 0);
            if (sendResult == SOCKET_ERROR)
            {
                std::cerr << "Failed to send data: " << GetLastErrorAsString() << std::endl;
                closesocket(childSocket);
                //return 1;
            }

            else
            {
                total++;
                cout << "\n succesfully connected!";
            }
        }
        // Receive data from the client
    }
    int status = 0;
    int by = send(peer_socket, (char*)&status, sizeof(status), 0);
    cout << "\n lobby status sent to the server==>"<<by;

    // Close the socket in the child process
    closesocket(childSocket);
    closesocket(peer_socket);
    // Clean up
    WSACleanup();
while(1)
{

}
    return 0;
}
