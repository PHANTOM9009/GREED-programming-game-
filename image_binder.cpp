
#include<iostream>
#include<SFML/Graphics.hpp>
using namespace std;

int main()
{
	sf::Image img;
	img.create(320, 80, sf::Color(0, 0, 0, 0));
	sf::Image img1;
	img1.loadFromFile("map_1 attributes/c0.png");
	sf::Image img2;
	img2.loadFromFile("map_1 attributes/c1.png");
	
	sf::Image img3;
	img3.loadFromFile("map_1 attributes/c2.png");
	sf::Image img4;
	img4.loadFromFile("map_1 attributes/c3.png");
	

	img.copy(img1, 0, 0);
	img.copy(img2, 80, 0);
	
	img.copy(img3, 160, 0);
	img.copy(img4, 240, 0);
	
	
	
	img.saveToFile("map_1 attributes/cannon_set.png");
}
