
#include<iostream>
#include<SFML/Graphics.hpp>
using namespace std;

int main()
{
	//the order is N S E W NE SE NW SW
	sf::Image img;
	img.create(640, 80, sf::Color(0, 0, 0, 0));
	sf::Image img0;
	img0.loadFromFile("map_1 attributes/Ships/Sprite-N4.png");
	sf::Image img1;
	img1.loadFromFile("map_1 attributes/Ships/Sprite-0004.png");
	sf::Image img2;
	img2.loadFromFile("map_1 attributes/Ships/Sprite-E4.png");
	
	sf::Image img3;
	img3.loadFromFile("map_1 attributes/Ships/Sprite-W4.png");
	sf::Image img4;
	img4.loadFromFile("map_1 attributes/Ships/Sprite-NE4.png");
	
	sf::Image img5;
	img5.loadFromFile("map_1 attributes/Ships/Sprite-SE4.png");

	sf::Image img6;
	img6.loadFromFile("map_1 attributes/Ships/Sprite-NW4.png");

	sf::Image img7;
	img7.loadFromFile("map_1 attributes/Ships/Sprite-SW4.png");

	img.copy(img0, 0, 0);
	img.copy(img1, 80, 0);	
	img.copy(img2, 160, 0);
	img.copy(img3, 240, 0);
	img.copy(img4, 320, 0);
	img.copy(img5, 400, 0);
	img.copy(img6, 480, 0);
	img.copy(img7, 560, 0);
	
	
	img.saveToFile("map_1 attributes/ship6_set.png");
}
