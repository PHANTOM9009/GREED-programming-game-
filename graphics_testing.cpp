#include<SFML/Graphics.hpp>
#include<iostream>

using namespace std;
vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
int cx(int x)
{
	return ((x * modes[11].width) / 1920);
}
int cy(int y)
{
	return ((y * modes[11].height) / 1080);
}
void resizeImage( sf::Image& originalImage, sf::Image& resizedImage)//code to resize the image
{
	const sf::Vector2u originalImageSize{ originalImage.getSize() };
	const sf::Vector2u resizedImageSize{ resizedImage.getSize() };
	for (unsigned int y{ 0u }; y < resizedImageSize.y; ++y)
	{
		for (unsigned int x{ 0u }; x < resizedImageSize.x; ++x)
		{
			unsigned int origX{ static_cast<unsigned int>(static_cast<double>(x) / resizedImageSize.x * originalImageSize.x) };
			unsigned int origY{ static_cast<unsigned int>(static_cast<double>(y) / resizedImageSize.y * originalImageSize.y) };
			resizedImage.setPixel(x, y, originalImage.getPixel(origX, origY));
		}
	}
}
void resizeTexture(sf::Texture& tex)
{
	sf::Image img = tex.copyToImage();
	sf::Image img1;
	
	img1.create(::cx(tex.getSize().x), ::cy(tex.getSize().y));
	//img1.create(40, 40);

	resizeImage(img, img1);

	tex.loadFromImage(img1);

}
int main()
{
	for (int i = 0; i < modes.size(); i++)
	{
		cout << modes[i].width << " " << modes[i].height<<endl;
    }
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "graphics testing");
	window.setFramerateLimit(60);
	sf::Texture tex;
	tex.loadFromFile("map_1 attributes/ship_s.png");
	resizeTexture(tex);
	
	sf::Sprite sp;
	sp.setTexture(tex);
	window.setFramerateLimit(60);
	sf::Clock clock;
	sf::Time time;
	int count = 0;
	while (window.isOpen())
	{
		count++;
		time += clock.restart();
		if (time.asSeconds() <= 1 && time.asSeconds() >= 1.15)
		{
			cout << count << endl;
			count = 0;
			time = sf::Time::Zero;
		}
		sf::Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))

			{
				window.close();

			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
			{
				window.setSize(sf::Vector2u(1920, 1080));
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}
		}
		
	}
	
}