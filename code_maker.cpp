//this program creates code for the maze

#include<iostream>
#include<fstream>
#include "List.h"
#include<math.h>
using namespace std;
//using the file named position.dat
class file
{
public:
    int id;
    float x;
    float y;
    void setter(int i, float xx, float yy)
    {
        id = i;
        x = xx;
        y = yy;
    }
};
int main()
{
    ifstream in("position1.dat", ios::binary | ios::in);
    file s;
    List<file> listing;
    while (in)
    {
        in.read((char*)&s, sizeof(s));
        if (in.eof())
            break;
        listing.add_rear(s);
        //cout<<s.id<<"\t"<<s.x<<"\t"<<s.y<<endl;
    }
    in.close();
    file temp;
    cout << "\n sorting....";
    for (int i = 0; i < listing.howMany() - 1; i++)
    {
        for (int j = 0; j < listing.howMany() - i - 1; j++)
        {
            if (listing[j].y >= listing[j + 1].y)
            {
                temp = listing[j];
                listing[j] = listing[j + 1];
                listing[j + 1] = temp;
            }
        }
    }
    //

    cout << "\n sorting the x- coordinate for the same y coordinate:\n";
    for (int i = 0; i < listing.howMany() - 1; i++)
    {
        for (int j = 0; j < listing.howMany() - i - 1; j++)
        {
            if (listing[j].x >= listing[j + 1].x && (abs(listing[j].y - listing[j + 1].y) == 1 || abs(listing[j].y - listing[j + 1].y) == 0))
            {
                temp = listing[j];
                listing[j] = listing[j + 1];
                listing[j + 1] = temp;
            }
        }
    }
    cout << "\n sorted data is:\n";
    cout << "\n adding the sorted data into the file";
    ofstream out;
    out.open("position_sorted.dat", ios::binary | ios::out);
    for (int i = 0; i < listing.howMany(); i++)
    {
        out.write((char*)&listing[i], sizeof(listing[i]));
    }
    out.close();
    in.open("position_sorted.dat", ios::binary | ios::in);
    while (in)
    {
        in.read((char*)&s, sizeof(s));
        if (in.eof())
            break;
        cout << s.id << "\t" << s.x << "\t" << s.y << endl;
    }
    in.close();





}
