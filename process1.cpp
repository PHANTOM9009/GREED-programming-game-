#include<iostream>
#include<Windows.h>
using namespace std;
/*
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
	//system("run.exe");
	
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	//LPTSTR* arg = { "hello.exe" };
	char commandLine[] = "\"F:\\current projects\\GREED(programming game)\\GREED(programming game)\\mock.exe\" 8080";
	if (!CreateProcessA(NULL,commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		cout<<"Error creating process"<<GetLastErrorAsString();
		
		return 1;
	}
	else
	{
		cout << "Process created";
		//return 0;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	cout << "\n child completed";
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
}