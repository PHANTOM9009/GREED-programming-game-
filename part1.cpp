#include<iostream>
using namespace std;
class one
{
public:
	one()
	{
		cout << "\n hello";
	}
	one(int a)
	{
		cout << "\n " << a;
	}
};
int main()
{
	one* ab = new one(2);

}