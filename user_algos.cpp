#include<iostream>
#include<vector>
#include<pybind11/pybind11.h>
#include<pybind11/stl.h>

using namespace std;
int a;
vector<int> return_vector(int size,vector<int> &vec)
{
	vector<int> ans;
	for (int i = 0; i < vec.size(); i++)
	{
		ans.push_back(vec[i]);
	}
	a = 10;
	return ans;
}

PYBIND11_MODULE(example, m) {
	m.def("return_vector", &return_vector, pybind11::arg("size"), pybind11::arg("vec"));
}