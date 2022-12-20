#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<stdio.h>
#include<string>
#include<sstream>
using namespace std;
/*
* diff between files is as follows
* 1. files are compared word by word and a line is one quanta
* 4. spaces are ignored completely
*/
vector<string> getWords(string& s)
{
	stringstream ss(s);
	vector<string> words;
	string ch;
	while (ss >> ch)
	{
		words.push_back(ch);
	}
	return words;
}
int main()
{
	string f1;
	string f2;
	cout << "\n enter the path of the first file==>";
	cin >> f1;
	cout << "\n enter the path of the second file==>";
	cin >> f2;
	f1 = "F:\\sample.txt";
	f2 = "F:\\sample.txt";
	ifstream f1in(f1, ios::in);
	ifstream f2in(f2, ios::in);
	vector<string> file1;//content of file1 line by line
	vector<string> file2;//content of file2 line by line
	while (f1in)
	{
		char ch[1000];
		f1in.getline(ch,1000, '\n');
		string ch1 = ch;
		if (ch1!="")
		{
			file1.push_back(ch1);
		}
	}
	while (f2in)
	{
		char ch[1000];
		f2in.getline(ch, 1000, '\n');
		string ch1 = ch;
		if (ch1!="")
		{
			file2.push_back(ch1);
		}
	}
	cout << "\n contents of file1==>\n";
	for (int i = 0; i < file1.size(); i++)
	{
		cout << file1[i] << endl;
	}
	cout << "\n contents of file2==>\n";
	for (int i = 0; i < file2.size(); i++)
	{
		cout << file2[i] << endl;
	}
	int maxi = max(file1.size(), file2.size());
	for (int i = 0; i < maxi; i++)
	{

	}

}