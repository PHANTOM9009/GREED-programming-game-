/*this version is the final version of the game server, this doesn't show the graphics, only does the computation*/
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


/* execution process of master, child and messenger is
* first run messenger
* run child process and take its process id and enter it in duplicate socket in master
* run master process
* 
*/
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
    hints.ai_socktype = SOCK_STREAM;

    hints.ai_flags = AI_PASSIVE;
	struct addrinfo* bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);
	SOCKET listenSocket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(listenSocket))
	{
		cout << "\n socket not created==>" << GETSOCKETERRNO();
	}

	int option = 0;
	if (setsockopt(listenSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&option, sizeof(option)))//for accepting ipv6 as well
	{
		cout << "\n problem in setting the flag==>";
	}
	int yes = 1;
	if (setsockopt(listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes)) < 0) //disabling nagle's algorithm for speed in sending the data
	{
		fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO());
	}


	cout << "\n binding the socket==>";
	if (bind(listenSocket, (const sockaddr*)bind_address->ai_addr, (int)bind_address->ai_addrlen))
	{
		cout << "\n failed to bind the socket==>" << GETSOCKETERRNO();
	}


    // Listen for incoming connections
    if (listen(listenSocket, 20) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen for connections: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        return 1;
    }

    // Accept a connection from a client whose socket we have to send
    struct sockaddr_storage clientAddr;
    socklen_t client_len= sizeof(clientAddr);
    SOCKET clientSocket = accept(listenSocket,(sockaddr*)&clientAddr,&client_len);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept connection: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        return 1;
    }
    cout << "\n connection accepted\n";
    //accept the peer socket to whom you will send the stuff;
    struct sockaddr_storage client_address;
    socklen_t client_len1 = sizeof(client_address);
    SOCKET peer_socket = accept(listenSocket, (sockaddr*)&client_address, &client_len1);
    int id;
    int bytes = recv(peer_socket, (char*)&id, sizeof(id), 0);

    //sending 
    WSAPROTOCOL_INFO protocolInfo;
    if (WSADuplicateSocket(clientSocket,id, &protocolInfo) != 0) {
        std::cerr << "Failed to duplicate socket handle: " << WSAGetLastError() << std::endl;
        cout << GetLastErrorAsString();
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    //now send over protocolInfo to the peer socket

    int byte = send(peer_socket, (char*)&protocolInfo, sizeof(protocolInfo), 0);
    cout << "\n bytes sent to the peer socket";
    while(1)
    { }


    /*
    // Create a new process
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Duplicate the socket handle
    WSAPROTOCOL_INFO protocolInfo;
    if (WSADuplicateSocket(clientSocket, GetCurrentProcessId(), &protocolInfo) != 0) {
        std::cerr << "Failed to duplicate socket handle: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Pass the socket handle to the child process
    

    // Convert command line arguments to wide-character strings
    std::wstring appNameW = L"F:\\current projects\\GREED(programming game)\\GREED(programming game)\\child.exe";
    std::wstring arg1W = std::to_wstring(protocolInfo.dwCatalogEntryId);
   
    cout << "\n id=>" << protocolInfo.dwCatalogEntryId;

    // Build command line string
    std::wstring cmdLineW = appNameW + L" " + arg1W;

    // Convert command line string to LPWSTR
    LPWSTR cmdLineWPtr = const_cast<LPWSTR>(cmdLineW.c_str());

    if (!CreateProcess(NULL,cmdLineWPtr, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &siStartInfo, &piProcInfo))
    {
        std::cerr << "Failed to create child process: " << GetLastError() << std::endl;
        cout << "\n" << GetLastErrorAsString();
        return 1;
    }

    // Wait for the child process to exit
    Sleep(100);
    DWORD status = 0;
    GetExitCodeProcess(piProcInfo.hProcess, &status);
    cout << status;
    if ((DWORD)status == STILL_ACTIVE)
    {
        cout << "\nstill active";
    }

    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    // Close the socket in the parent process
    closesocket(clientSocket);

    // Clean up
   
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    */
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
