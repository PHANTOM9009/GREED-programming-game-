
#include<iostream>
#include<SFML/Graphics.hpp>
using namespace std;

int main()
{
	//the order is N S E W NE SE NW SW
	sf::Image img;
	img.create(560, 80, sf::Color(0, 0, 0, 0));
	
	sf::Image img0;
	img0.loadFromFile("map_1 attributes/land.png");
	
	sf::Image img1;
	img1.loadFromFile("map_1 attributes/water_sprite.png");

	img.copy(img0, 0, 0);
	img.copy(img1, 80, 0);	
	
	
	
	img.saveToFile("map_1 attributes/water_set2.png");
}
