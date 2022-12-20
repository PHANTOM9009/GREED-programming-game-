#include<iostream>
#include<Windows.h>
using namespace std;
//program to call a child process..................
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
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
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	//LPTSTR* arg = { "hello.exe" };
	
	if (!CreateProcess(L"F:\\current projects\\GREED(programming game)\\GREED(programming game)\\GREED(programming game).exe",NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		cout<<"Error creating process"<<GetLastErrorAsString();
		return 1;
	}
	else
	{
		cout << "Process created";
		return 0;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	cout << "\n child completed";
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
}