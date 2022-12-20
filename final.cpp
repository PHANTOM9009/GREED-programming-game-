#include<iostream>
#include<SFML/Graphics.hpp>
#include<fstream>
#include<Windows.h>
using namespace std;
void putEscapeSeq(string& pb)
{
	int l = pb.length();
	for (int i = 0; i < l; i++)
	{
		if (pb[i] == '\\')
		{
			auto it = pb.begin();
			advance(it, i + 1);
			pb.insert(it, '\\');
			i++;
		}
	}
}
bool compileFile(string filename)
{
	char pBuf[256];
	size_t length = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, length);
	cout << "\n file path is==>" << pBuf;
	string pb = pBuf;
	cout << "\n directory name is==>" << getDirName(pb);
	putEscapeSeq(pb);
	string one = "f: && cd" + pb;
	char en[1000];
	strcpy(en, pb.c_str());
	cout << "\n the new path is==>" << pb;
	
	string navigate = "f: && cd \"F:\\GREED(programming game)\\GREED(programming game)\" && call \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64\"";
	filename.append(" ");
	filename.insert(filename.begin(), ' ');
	string compile = " && cl /MD -I\"F:\\GREED(programming game)\\user_libs\"" + filename + "/link /LIBPATH:\"F:\\GREED(programming game)\\user_libs\" lib2.lib > log.txt";
	string combine = navigate + compile;
	char cmd1[1000];
	strcpy(cmd1, combine.c_str());
	cout << "\n the entire command is==>" << cmd1;
	system(cmd1);
	//now openening the file
	ifstream in("log.txt", ios::in);
	char buff[100];
	while (in)
	{
		in >> buff;

		if (strcmp(buff, "LNK1561:") == 0)
		{
			in.close();
			return true;
		}

	}
	in.close();
	return false;
}
int main()
{
	/*
	//cout<<system("del F:/user_algos.cpp");
	//const char* ch= "copy D:\\user_algos\\user_algos.cpp F:\\user_algos.cpp";
	cout<<system("f: && cd \"F:\\GREED(programming game)\\GREED(programming game)\" && call \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64\" && cl /EHsc /MT -I\"F:\\GREED(programming game)\\final_libs\" /MD -I\"C:\\SFML visual c++\\SFML-2.5.1\\include\" -I\"C:\\TGUI-0.10-nightly\\include\" \"F:\\GREED(programming game)\\GREED(programming game)\\greed.cpp\" /link /LIBPATH:\"F:\\GREED(programming game)\\final_libs\" /LIBPATH:\"C:\\SFML visual c++\\SFML-2.5.1\\lib\" /LIBPATH:\"C:\\TGUI-0.10-nightly\\lib\"  lib2.lib tgui.lib sfml-graphics.lib sfml-audio.lib sfml-network.lib sfml-window.lib sfml-system.lib");
	//cout << system("call \"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64\" && cl /EHsc   -I\"C:/TGUI - 0.10 - nightly/include\"   \"F:/GREED(programming game)/GREED(programming game)/testing.cpp\"  /link /LIBPATH:\"C:/TGUI - 0.10 - nightly/lib\"  tgui.lib ");
	//cout<<system("g++ -o F:\\GREED(programming game)\\GREED(programming game)\\user_algos F:\\GREED(programming game)\\GREED(programming game)\\user_algos.cpp 2> F:\\output_log.txt");
	/*
	ifstream in("F:\\output_log.txt", ios::in);
	char buff[5000];
	while (in)
	{
		in.read(buff, sizeof(buff));


	}
	in.close();
	cout << buff;

	*/
	//writing the  code to compile the code file passed by the user..
	char pBuf[256];
	size_t length = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, length);
	
	cout << "\n file path is==>" << pBuf;
	string pb = pBuf;//path with escape sequence set
	putEscapeSeq(pb);
	cout << "\n the new file path is==>" << pb;
	string filename = "user_algos.cpp";
		cout << "\n file compiled or not==>" << compileFile(filename);
	
}