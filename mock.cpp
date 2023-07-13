#include <iostream>
#include<string>
using namespace std;
int main(int argc, char* argv[])
{
    if (argc > 0)
    {
        if (argc > 1)
        {
            int value = std::stoi(argv[1]);
            std::cout << "Received value from the parent process: " << value << std::endl;
        }
        cout << "\n 0th value is=>" << argv[0];
    }
    while(1)
    { }
    return 0;
}