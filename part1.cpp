#include<iostream>
using namespace std;
class check
{
public:
	int a;
	double b;
};
int main()
{
	check ob;
	ob.a = 2;
	ob.b = 3;
	char buff[12];
	char* p = buff;
	p = (char*)&ob;
	check* ob2 = (check*)p;
	
	check ob1;
	char p1[12];
	memcpy(p1, p, 4);
	memcpy(p1 + 4, p, 4);
	memcpy(&ob1, p1, 8);
	
	cout << ob1.a << " " << ob1.b;
}