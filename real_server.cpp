#include<iostream>
#include<unordered_map>
#include<algorithm>
using namespace std;
int main()
{
	unordered_map<int, int> ob;
	ob[10] = 1;
	ob[20] = 2;
	ob[1] = 3;
	ob[3] = 4;

	for (int i = 0; i < ob.size(); i++)
	{
		auto it = ob.begin();
		advance(it,i);
		
		if (it->first == 10)
		{
			ob.erase(it);
			//i--;
			continue;
			//i--;
		}
		
		cout << it->second;
	}
}