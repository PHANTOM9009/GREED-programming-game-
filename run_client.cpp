// lib2.cpp : Defines the functions for the static library.
//


#pragma once
#include<SFML/Graphics.hpp>

#include<future>
#include<thread>

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() WSAGetLastError()


#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif
#include "lib2.hpp"

using namespace std;

long Event::count = 0;
int  ship::total = 0;
List<Greed::vertex> ship::globalMap;
List<Greed::cannon> ship::cannon_list;
deque<ship*> Control::ship_list;
List<Greed::coords> Control::bonus;
List<Greed::coords> Control::storm;
List<Greed::coords> Control::opaque_coords;
List<Greed::cannon> Control::cannon_list;
long double graphics::total_secs = 0;
bool graphics::showPatterns = false;


//for moving entites over the screen
template<typename T>
class mover_tgui
{
public:
	void moving(sf::RenderWindow& window, tgui::Gui& gui, T button)
	{

		bool mouseClicked = false;
		bool dragging = false;
		sf::Vector2i mousepos;

		int x2, y2, x1, y1, X, Y;
		int m, n;
		int run = 0;
		int r = 0;
		int o = 0;

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();

				}

				if ((event.type == sf::Event::MouseButtonPressed) && (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
				{
					mouseClicked = true;
					//
					//cout<<mousepos.x<<" "<<mousepos.y<<endl;
					mousepos = sf::Mouse::getPosition(window);
					if ((button->getPosition().x <= mousepos.x && (button->getPosition().x + button->getSize().x) >= mousepos.x) && button->getPosition().y <= mousepos.y && (button->getPosition().y + button->getSize().y) >= mousepos.y)
					{
						dragging = true;
						x2 = event.mouseButton.x - button->getPosition().x;
						y2 = event.mouseButton.y - button->getPosition().y;

					}

				}

				if (event.type == sf::Event::MouseButtonReleased)
				{
					mouseClicked = false;
					dragging = false;
				}


				if (event.type == sf::Event::MouseMoved)
				{
					m = event.mouseMove.x;
					n = event.mouseMove.y;

				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					o = 1;
					cout << "\n the final position is==>" << button->getPosition().x << " " << button->getPosition().y;
					break;
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
				{

					cout << "\n middle";
				}

				gui.handleEvent(event); // Pass the event to the widgets
			}

			if (mouseClicked == true && dragging == true)
			{
				//cout<<m<<" "<<n<<endl;

				button->setPosition(sf::Mouse::getPosition(window).x - x2, sf::Mouse::getPosition(window).y - y2);
				//rect.setPosition(m-x2,n-y2);
				run++;
				r = 1;

			}
			else if (mouseClicked == false && dragging == false && run > 0 && r == 1)
			{
				cout << button->getPosition().x << " " << button->getPosition().y << endl;
				r = 0;
			}
			if (o == 1)
			{
				break;
			}
			window.clear(sf::Color::Cyan);
			//window.draw(button);
			gui.draw();
			window.display();

		}


	}


};



vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
int shall = 0;
int cx(int x)
{
	return round(((x * modes[shall].width) / 1920));
}
int cy(int y)
{
	return round(((y * modes[shall].height) / 1080));
}
int rx(int x)
{
	return round(((x * 1920) / modes[shall].width));
}
int ry(int y)
{
	return round(((y * 1080) / modes[shall].height));
}

sf::RenderWindow window(sf::VideoMode(::cx(1970), ::cy(1190)), "GREED");

template<typename Y>
int getIndex(List<Y>& l, Y val)
{
	int index = -1;
	for (int i = 0; i < l.howMany(); i++)
	{
		if (l[i] == val)
		{
			index = i;
			break;
		}
	}
	return index;
}


void resizeImage(sf::Image& originalImage, sf::Image& resizedImage)//code to resize the image
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

void Greed::deleteDuplicate(List<abs_pos>& ob)
{
	int temp = ob.howMany();
	for (int i = 0; i < ob.howMany() - 1; i++)
	{
		if (ob[i].x == ob[i + 1].x && ob[i].y == ob[i + 1].y)
		{
			ob.delAt(i + 1);
		}
	}
}
List<Greed::abs_pos> Greed::makeList(List<coords> ob) //makes a list of continous coordinates in x and y using the row and column information.
{
	List<abs_pos> l;

	for (int i = 0; i < ob.howMany() - 1; i++)
	{


		if (ob[i].r - ob[i + 1].r == 0)//row is constant i.e only x axis has to be changed
		{
			if (ob[i + 1].c > ob[i].c)//if column increases i.e x axis increases
			{
				for (int j = ob[i].c * len; j <= ob[i + 1].c * len; j++)
				{
					l.add_rear(abs_pos(j, ob[i].r * breadth));
				}
			}
			else if (ob[i].c > ob[i + 1].c)
			{
				for (int j = ob[i].c * len; j >= ob[i + 1].c * len; j--)
				{
					l.add_rear(abs_pos(j, ob[i].r * breadth));
				}
			}
		}
		else if (ob[i].c - ob[i + 1].c == 0)
		{
			if (ob[i + 1].r > ob[i].r)
			{
				for (int j = ob[i].r * breadth; j <= ob[i + 1].r * breadth; j++)
				{
					l.add_rear(abs_pos(ob[i].c * len, j));
				}
			}
			else if (ob[i + 1].r < ob[i].r)
			{
				for (int j = ob[i].r * breadth; j >= ob[i + 1].r * breadth; j--)
				{
					l.add_rear(abs_pos(ob[i].c * len, j));
				}
			}
		}
		else//for diagonal elements
		{
			Greed::abs_pos tempo;
			//for north east first
			if (ob[i].r - ob[i + 1].r == 1 && ob[i + 1].c - ob[i].c == 1)
			{
				int j = ob[i].r * breadth;
				int k = ob[i].c * len;
				while (j != ob[i + 1].r * breadth && k != ob[i + 1].c * len)
				{
					l.add_rear(abs_pos(k, j));
					j--;
					k++;
				}
				tempo = abs_pos(k, j);

			}
			//for south east 
			else if (ob[i + 1].c - ob[i].c == 1 && ob[i + 1].r - ob[i].r == 1)
			{

				int j = ob[i].r * breadth;
				int k = ob[i].c * len;
				while (j != ob[i + 1].r * breadth && k != ob[i + 1].c * len)
				{
					l.add_rear(abs_pos(k, j));
					k++;
					j++;
				}
				tempo = (abs_pos(k, j));

			}
			else if (ob[i + 1].r - ob[i].r == 1 && ob[i].c - ob[i + 1].c == 1)//for south west
			{

				int j = ob[i].r * breadth;// y axis
				int k = ob[i].c * len;//x axis
				while (j != ob[i + 1].r * breadth && k != ob[i + 1].c * len)
				{
					l.add_rear(abs_pos(k, j));
					k--;
					j++;
				}
				tempo = (abs_pos(k, j));

			}
			//for north west
			else if (ob[i].r - ob[i + 1].r == 1 && ob[i].c - ob[i + 1].c == 1)
			{
				int j = ob[i].r * breadth;
				int k = ob[i].c * len;
				while (j != ob[i + 1].r * breadth && k != ob[i + 1].c * len)
				{
					l.add_rear(abs_pos(k, j));
					k--;
					j--;
				}
				tempo = (abs_pos(k, j));
			}
			l.add_rear(Greed::abs_pos(tempo));
		}
	}
	deleteDuplicate(l);
	return l;
}
int Greed::smallest_entry(List<A>& open)
{
	A temp = open[0];
	int index = 0;
	for (int i = 0; i < open.howMany(); i++)
	{
		if (temp.sum > open[i].sum)
		{
			temp = open[i];
			index = i;

		}
	}
	return index;
}
int Greed::isThere(coords b, List<A>& closed)//returns the index //for class A
{
	for (int i = 0; i < closed.howMany(); i++)
	{
		if (b == closed[i].v)
			return i;
	}
	return -1;

}
List<Greed::abs_pos> Greed::makeTrajectory(double x1, double y1, double x2, double y2)//the function which makes the trajectory of the bullet fired
{
	//(x1,y1) coordinates of the launch ship, (x2,y2) are the coordinates of the target ship
	List<Greed::abs_pos> traject;

	double dx = abs(x1 - x2);
	double dy = abs(y1 - y2);
	double step;
	if (dx > dy)
	{
		step = dx;
	}
	else if (dx < dy)
	{
		step = dy;
	}
	else
	{
		step = dx;//in case that they are same
	}
	double xinc = dx / step;
	double yinc = dy / step;

	double xx = x1;
	double yy = y1;
	for (int i = 1; i <= step + 100; i++)
	{
		if (x1 - x2 > 0)
		{
			xx -= xinc;
		}
		else if (x1 - x2 < 0)
		{
			xx += xinc;
		}
		if (y1 - y2 > 0)
		{
			yy -= yinc;
		}
		else if (y1 - y2 < 0)
		{
			yy += yinc;
		}

		traject.add_rear(Greed::abs_pos(round(xx), round(yy)));
	}
	return traject;
}
int Greed::find_index(coords b, List<vertex> graph)//for vertex class only
{
	for (int i = 0; i < graph.howMany(); i++)
	{
		if (b == graph[i].b)
			return i;
	}
	return -1;
}
void Greed::find_path(coords origin, coords goal, List<A>& closed, Greed::path_attribute& ob, List<vertex>& rmap)//takes in the closed list and returns the path
{

	ob.target.add_front(goal);

	int index = isThere(goal, closed);


	ob.resources += rmap[closed[index].v.r * columns + closed[index].v.c].b.cost;// finds the coordinate from the original map and finds the real cost that the ship will have to pay to  take that path

	if (index != -1 && !(closed[index].v == origin))
	{
		find_path(origin, closed[index].path, closed, ob, rmap);

	}
	else
		return;


}

void Greed::path(coords origin, coords goal, List<Greed::vertex>& graph, path_attribute& ob, List<Greed::vertex>& r_graph)//this function returns the path in rows and columns to be followed in the map given the origin and the goal node
{
	//r_graph is the graph of the original static map from which resources have to be subtracted
	if (graph[goal.r * columns + goal.c].b.tile_id != Entity::CANNON && graph[goal.r * columns + goal.c].b.tile_id != Entity::LAND)
	{
		List<A> open;//open list for A*
		List<A> closed;//closed list for A*
		//starting node is 00
		graph[find_index(origin, graph)].b.cost = 0;
		graph[find_index(origin, graph)].b.heuristic = calc_heuristic(graph[find_index(origin, graph)].b, graph[find_index(goal, graph)].b);

		open.add_rear(A(graph[find_index(origin, graph)].b, graph[find_index(origin, graph)].b.cost, graph[find_index(origin, graph)].b.heuristic, graph[find_index(origin, graph)].b));
		int success = 0;
		while (open.howMany() > 0)
		{
			//finding the smallest sum from the open list
			int temp = smallest_entry(open);
			//cout<<"\n chosen node is==>"<<open[temp].v.r<<open[temp].v.c<<endl;
			//if chosen node is the goal node  then break
			if (open[temp].v == graph[find_index(goal, graph)].b)
			{
				closed.add_rear(A(open[temp].v, open[temp].cost, open[temp].heuristic, open[temp].path));
				open.delAt(temp);
				success = 1;

				break;
			}

			//taking the connections of the current node from the graph
			int index = find_index(open[temp].v, graph);//formula is [ number of columns*row+column]

			for (int i = 0; i < graph[index].vert.howMany(); i++)
			{
				int cost = open[temp].cost + graph[index].vert[i].ob->cost;
				//checking if its present in closed list

				int ind = isThere(coords(graph[index].vert[i].ob->r, graph[index].vert[i].ob->c, graph[index].vert[i].ob->cost), closed);

				//if the cost_so_far for the current node is better found then update it in the table
				int ind1 = isThere(coords(graph[index].vert[i].ob->r, graph[index].vert[i].ob->c, graph[index].vert[i].ob->cost), open);
				//cout<<"\n neighbours are==>\n";
				//cout<<graph[index].vert[i].r<<graph[index].vert[i].c<<endl;
				if (ind != -1 && cost < closed[ind].cost)
				{
					//cout<<"\n worked for==>"<<closed[ind].v.r<<closed[ind].v.c<<endl;
					closed[ind].cost = cost;
					//remove from the closed list and add in the open list
					//update the path
					closed[ind].path = open[temp].v;
					open.add_rear(A(closed[ind].v, closed[ind].cost, closed[ind].heuristic, closed[ind].path));
					closed.delAt(ind);
				}


				//checking if the connection is present in the open list


				else   if (ind1 != -1 && cost < open[ind1].cost)
				{
					open[ind1].cost = cost;
					open[ind1].path = open[temp].v;
					open[ind1].sum = cost + open[ind1].heuristic;
				}
				else//if the connection is not present in either of the list then add it in the open list with the correct parameters
				{
					//first finding the heuristic value
					if (ind1 == -1 && ind == -1)
					{

						graph[index].vert[i].ob->heuristic = calc_heuristic(coords(graph[index].vert[i].ob->r, graph[index].vert[i].ob->c, graph[index].vert[i].ob->cost), graph[find_index(goal, graph)].b);

						open.add_rear(A(coords(graph[index].vert[i].ob->r, graph[index].vert[i].ob->c, graph[index].vert[i].ob->cost), cost, graph[index].vert[i].ob->heuristic, open[temp].v));//the last parameter could be graph[index].b
					}

				}
			}
			//we have finished looking for the connections now remove the current node from open list and add it to the closed list
			closed.add_rear(A(open[temp].v, open[temp].cost, open[temp].heuristic, open[temp].path));
			open.delAt(temp);

		}
		//now the  closed list will have the required path....

		//determining the net path
		if (success == 1)
		{

			find_path(origin, goal, closed, ob, r_graph);


			//calculating the cost according to the original map


		}
	}
}

void Greed::getMap(int code[rows][columns], int r, int c, List<Greed::vertex>& graph)//takes in pointer to the code of the maze // here its 3*3 but has to be changed
{
	int n = 0;

	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			/**
				two tile id's are only possible:
				For water:
				water can be storm and bonus
				this distinguishing has to be done at the time of initializing the map and this will be updated time to time
			**/
			/**
			for Land
				a land can be:
				1. a simple land
				2. a cannon
				or a bonus
				this distinguishing has to be done at the time of initializing of the map and this will be updated time to time

			**/
			if (code[i][j] == 1)//code 1 means water is there
			{
				graph.add_rear(vertex(coords(i, j, WATER_COST, Entity::WATER, -1, Bonus::NA)));
			}
			else if (code[i][j] == 0)//if it is impenetrable land
			{
				graph.add_rear(vertex(coords(i, j, -1, Entity::LAND, -1, Bonus::NA)));
			}

		}
	}

	int ind, ind1;
	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			int n = 0, s = 0, e = 0, w = 0;
			if (code[i][j] == 1)
			{
				//finding the correct object index according to the row and column of the loops
				 //ind=find_index(coords(i,j),graph);
				ind = i * columns + j;
				if ((i - 1) >= 0 && code[i - 1][j] == 1)//checking upper box //NORTH
				{

					//ind1=find_index(coords(i-1,j),graph);
					ind1 = (i - 1) * columns + j;
					graph[ind].vert.add_rear(point(&graph[ind1].b));
					n = 1;

				}
				if ((j + 1) <= (c - 1) && code[i][j + 1] == 1)//going to the right // checking for East
				{
					e = 1;
					//ind1=find_index(coords(i,j+1),graph);
					ind1 = i * columns + (j + 1);
					graph[ind].vert.add_rear(point(&graph[ind1].b));
				}
				if ((j - 1) >= 0 && code[i][j - 1] == 1)//going to the left //checking for West
				{
					w = 1;
					//ind1=find_index(coords(i,j-1),graph);
					ind1 = i * columns + (j - 1);
					graph[ind].vert.add_rear(point(&graph[ind1].b));
				}
				if ((i + 1) <= (r - 1) && code[i + 1][j] == 1)//going down //checking for south
				{
					s = 1;
					//ind1=find_index(coords(i+1,j),graph);
					ind1 = (i + 1) * columns + j;
					graph[ind].vert.add_rear(point(&graph[ind1].b));
				}

				//checking for north east
				if ((j + 1) <= (c - 1) && (i - 1) >= 0 && code[i - 1][j + 1] == 1 && n == 1 && e == 1)
				{
					ind1 = (i - 1) * columns + (j + 1);
					graph[ind].vert.add_rear(point(&graph[ind1].b));
				}
				//checking for south east
				if ((j + 1) <= (c - 1) && (i + 1) <= (r - 1) && code[i + 1][j + 1] == 1 && e == 1 && s == 1)
				{
					ind1 = (i + 1) * columns + (j + 1);
					graph[ind].vert.add_rear(point(&graph[ind1].b));
				}
				//checking for south west
				if ((i + 1) <= (r - 1) && (j - 1) >= 0 && code[i + 1][j - 1] == 1 && s == 1 && w == 1)
				{
					ind1 = (i + 1) * columns + (j - 1);
					graph[ind].vert.add_rear(point(&graph[ind1].b));
				}
				//checking for north west
				if ((i - 1) >= 0 && (j - 1) >= 0 && code[i - 1][j - 1] == 1 && n == 1 && w == 1)
				{
					ind1 = (i - 1) * columns + (j - 1);
					graph[ind].vert.add_rear(point(&graph[ind1].b));

				}


			}
		}
	}


}
void Map::intialize_graphics(sf::Texture& tex)
{
	if (map_id == 0)
	{
		tex.loadFromFile("map_1 attributes/map_set.png");
		resizeTexture(tex);
	}
}
void Map::initialize_map(int code[12][24], List<Greed::coords>& opaque)//list of opaque tiles
{
	file s;
	if (this->map_id == 0)//this is the first map
	{
		/*here in this map id's are as follows
		* 1. id of the water is 2
		* 2. id of the floor is 1
		*/
		list<int> c;
		ifstream in("position_sorted.dat", ios::binary | ios::in);
		while (in)
		{
			in.read((char*)&s, sizeof(s));
			if (in.eof())
			{
				break;
			}
			if (s.id == 2)//water
			{
				c.push_back(1);
			}
			else if (s.id == 1)//for land
			{
				c.push_back(0);

			}
		}
		in.close();

		//setting the code array
		list<int>::iterator it = c.begin();
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				code[i][j] = *it;
				it++;
			}
		}
		//setting the opaque tiles list
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				if (code[i][j] == 0)
				{
					opaque.add_rear(Greed::coords(i, j));
				}
			}
		}


	}

}

bool ship::isAuthenticPath(List<Greed::abs_pos> ob)//function to check if the list supplied is authentic or not
{

	int found = 1;
	for (int i = 0; i < ob.howMany() - 1; i++)
	{
		if (abs(ob[i].x - ob[i + 1].x) != 1 || abs(ob[i].y - ob[i + 1].y) != 1)
		{
			found = 0;
		}
	}
	if (found == 1)
	{
		return true;
	}
	else if (found == 0)
	{
		return false;
	}
}
void ship::filter(vector<Greed::abs_pos>& l, ship::boundingEntity& ob)
{
	//cout << "\n entity recieved is==>" << ob.x1 << " " << ob.x2 << endl << ob.y1 << " " << ob.y2;
	for (int i = 0; i < l.size(); i++)
	{
		if (l[i].x >= ob.x1 && l[i].x <= ob.x2 && l[i].y >= ob.y1 && l[i].y <= ob.y2)
			continue;
		else
		{
			auto it = l.begin();
			advance(it, i);
			l.erase(it);
			i--;
		}
	}
}
ship::boundingEntity ship::getRadiBound(Greed::coords position)//returns the entity bound of the radii 
{
	boundingEntity ob;
	int c1 = position.c - this->bullet_radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int c2 = position.c + this->bullet_radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int r1 = position.r - this->bullet_radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	int r2 = position.r + this->bullet_radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	ob.x1 = c1 * len;
	ob.x2 = c2 * len + len;
	ob.y1 = r1 * len;
	ob.y2 = r2 * len + len;

	ob.c1 = c1;
	ob.c2 = c2;
	ob.r1 = r1;
	ob.r2 = r2;
	return ob;



}
bool ship::collide(int s)//a function with a different approach to stop a ship when an collision is about to occur
{
	return true;
}
bool ship::isOverlapping(ship::shipEntity e1, ship::shipEntity e2, int axis)//axis=0 for x; axis=1 for y
{
	vector<Greed::abs_pos> pos;
	pos.push_back(e1.c1);
	pos.push_back(e1.c2);
	pos.push_back(e1.c3);
	pos.push_back(e1.c4);
	int found = 0;
	for (int i = 0; i < 4; i++)
	{
		if (axis == 0)//for x axis
		{
			if (e2.c1.x < pos[i].x && e2.c2.x > pos[i].x)
			{
				found = 1;
				break;
			}
		}
		else if (axis == 1)
		{
			if (e2.c1.y < pos[i].y && e2.c3.y > pos[i].y)
			{
				found = 1;
				break;
			}
		}
	}
	if (found == 0)
	{
		return false;
	}
	return true;
}
bool ship::collide(int s, Greed::coords& pos)//function to check if this->ship_id collided with a ship having ship id s
{
	//dont use getShipEntity without mutex, for now its been called in graphics which is protecting it with mutex
	shipEntity opp = shipInfoList[s].ob->getShipEntity();//its coordinates will be checked
	shipEntity my = this->getShipEntity_without_mutex();//its entity will be checked
	//cout << "\n opp entity received is==>" << opp.c1.x + 1 << " " << opp.c1.y + 1;
	if (this->ship_id == 0 && s == 2)
	{
		//cout << "\n opponents pos=>" << opp.c1.x << " " << opp.c1.y;
		//cout << "\n my pos=>" << my.c1.x << " " << my.c1.y;
	}
	//cutting sprites from the top and bottom
	if (isOverlapping(opp, my, 0) && isOverlapping(opp, my, 1))//for sideways collision(diagonally)
	{
		//cout << "\n both are overlapping";
		return true;
	}
	else if (isOverlapping(opp, my, 0) && my.c1.y == opp.c1.y && my.c3.y == opp.c3.y)//if overlapping is from left or right then y axis will be same
	{
		//cout << "\n x axis is overlapping";
		return true;
	}
	else if (isOverlapping(opp, my, 1) && my.c1.x == opp.c1.x && my.c2.x == opp.c2.x)
	{
		//cout << "\n y axis is overlapping";
		return true;
	}
	return false;


	/*
	if (my.c1.x < opp.c1.x + 1 && opp.c1.x + 1 < my.c2.x && my.c1.y < opp.c1.y + 1 && opp.c1.y + 1 < my.c4.y)//checking for c1 of opp
	{

		return true;
	}
	else if (my.c1.x < opp.c2.x - 1 && opp.c2.x - 1 < my.c2.x && my.c1.y < opp.c2.y + 1 && opp.c2.y + 1 < my.c4.y)//checking for c1 of opp
	{

		return true;
	}
	else if (my.c1.x < opp.c3.x - 1 && opp.c3.x - 1 < my.c2.x && my.c1.y < opp.c3.y - 1 && opp.c3.y - 1 < my.c4.y)//checking for c1 of opp
	{

		return true;
	}
	else if (my.c1.x < opp.c4.x + 1 && opp.c4.x + 1 < my.c2.x && my.c1.y < opp.c4.y - 1 && opp.c4.y - 1 < my.c4.y)//checking for c1 of opp
	{

		return true;
	}
	return false;
	*/

}
bool ship::checkCollision(int sid, const Greed::bullet& ob)//sid is the ship id of  the victim ship.
{
	Direction dir = shipInfoList[sid].getShipDirection();//direction of the target ship
	ship::shipEntity entity = shipInfoList[sid].ob->getShipEntity();//entity of the target ship
	sf::FloatRect bullet_pos = ob.bullet_entity.getGlobalBounds();
	Greed::abs_pos coord(::rx(bullet_pos.left) - origin_x, ::ry(bullet_pos.top) - origin_y);
	int dis = (::cx(10) + ::cy(10)) / 2;
	if (dir == Direction::EAST || dir == Direction::WEST)
	{

		if (coord.x >= entity.c1.x && coord.x <= entity.c2.x && coord.y >= entity.c1.y + dis && coord.y <= entity.c4.y - dis)
		{
			return true;
		}
		else
			return false;
	}
	else
	{

		if ((coord.y >= entity.c1.y && coord.y <= entity.c4.y) && (coord.x >= entity.c1.x + dis && coord.x <= entity.c2.x - dis))
		{
			return true;
		}
		else
		{
			return false;
		}
	}





}

int shipInfo::getTotalShips()
{
	return ob->getTotalShips();
}
int shipInfo::getShipId()
{
	return ob->getShipId();
}
int shipInfo::getShipRadius()
{
	return ob->getShipRadius();
}
double shipInfo::getCurrentHealth()
{
	return ob->getCurrentHealth();
}
double shipInfo::getCurrentGold()
{
	return ob->getCurrentGold();
}

double shipInfo::getCurrentFuel()
{
	return ob->getCurrentFuel();
}
int shipInfo::getDiedStatus()
{
	return ob->getDiedStatus();
}
Direction shipInfo::getShipDirection()
{
	return ob->getShipDirection();
}
Greed::coords shipInfo::getCurrentTile()
{
	return ob->getCurrentTile();
}
Greed::coords shipInfo::getCurrentRearTile()
{
	return ob->getCurrentRearTile();
}
Greed::abs_pos shipInfo::getRealAbsolutePosition()
{
	return ob->getRealAbsolutePosition();
}
//event class for the game
/// ////////////////////  //// ///// //// ////////// ///////  ////// //////  ////// //////  //////////  ////////  ///////  ////////
//defining event class for all the events happening in the game
vector<Greed::coords> ship::getRadiusCoords_cannon(int c_id)
{
	if (cannon_list.howMany() > c_id && c_id >= 0)
	{


		vector<Greed::coords> arr;
		int radius = 2;
		Greed::coords position;

		position = cannon_list[c_id].getCannonPosition();


		int c1 = position.c - radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int c2 = position.c + radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int r1 = position.r - radius;
		if (r1 < 0)
		{
			r1 = 0;
		}
		else if (r1 > rows)
		{
			r1 = rows - 1;
		}
		int r2 = position.r + radius;
		if (r2 < 0)
		{
			r2 = 0;
		}
		else if (r2 > rows)
		{
			r2 = rows - 1;
		}
		for (int i = r1; i <= r2; i++)
		{
			for (int j = c1; j <= c2; j++)
			{
				arr.push_back(Greed::coords(i, j));
			}
		}
		return arr;
	}

}
bool ship::frame_rate_limiter()
{

	sf::Time time = clock.restart();
	elapsed_time += time.asSeconds();
	int frame = (int)(elapsed_time * frame_rate_limit) % frame_rate_limit;
	if (elapsed_time > 1)
	{
		elapsed_time = 0;
	}
	if (frame != current_frame_no)
	{
		current_frame_no = frame;
		return true;
	}
	return false;



}

ship::ship()//default ctor for now
{
	frame_rate_limit = 200;
	elapsed_time = 0;
	current_frame_no = -1;
	current_time = chrono::high_resolution_clock::time_point();

	isFiring = false;
	health = 200;
	motion = 0;
	autopilot = 0;
	ship_id = total;
	mutex_id = total;
	total++;
	//making the localMap here
	died = 0;
	tile_pos_front = Greed::coords(5, 5);
	dir = Direction::NORTH;
	pointPath = -1;
	pointTilePath = 0;
	fuel = DEF_FUEL;
	ammo = DEF_AMMO;
	radius = 2;
	bullet_radius = radius;
	current_event_point = 0;
	bullet_pointer = -1;
	shield = 0;
	gold = INITIAL_GOLD;
	killer_ship_id = -1;
	killer_cannon_id = -1;
	threshold_health = 10;
	score = 0;
	minutes = INT_MAX;
	seconds = INT_MAX;


}
vector<int> ship::cannonsIamInRadiusOf()// a new functions which returns all the cannons whose radius the ship is in;
{
	Control ob;
	vector<int> l;
	List<Greed::cannon> cannon_list = ob.getCannonList(2369);
	for (int i = 0; i < cannon_list.howMany(); i++)
	{
		if (cannon_list[i].isDead == true)
		{
			continue;
		}
		//checking first for the front side of the ship
		bool front = cannon_list[i].isShipInMyRadius(this->ship_id, ShipSide::FRONT);
		bool rear = cannon_list[i].isShipInMyRadius(this->ship_id, ShipSide::REAR);
		if (front || rear)
		{
			l.push_back(cannon_list[i].cannon_id);
		}
	}
	return l;
}
vector<Greed::coords> ship::getRadiusCoords_ship(int s_id)
{
	auto cmp = [](Greed::coords a, Greed::coords b) {
		if (a.r == b.r)
		{
			return a.c < b.c;
		}
		if (a.c == b.c)
		{
			return a.r < b.r;
		}
		else
		{
			return a.r < b.r;
		}
	};
	std::map<Greed::coords, int, decltype(cmp)> arr(cmp);
	Control obb;
	deque<ship*> l1;
	l1 = obb.getShipList(2369);

	int radius = l1[s_id]->getShipRadius();
	Greed::coords position;

	position = l1[s_id]->getCurrentTile();//first doing with the front position of the ship


	int c1 = position.c - radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int c2 = position.c + radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int r1 = position.r - radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	int r2 = position.r + radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	for (int i = r1; i <= r2; i++)
	{
		for (int j = c1; j <= c2; j++)
		{
			arr.insert(pair<Greed::coords, int>(Greed::coords(i, j), 1));
		}
	}
	//now doing the same stuff with rear coordinates

	position = l1[s_id]->getCurrentRearTile();

	c1 = position.c - radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	c2 = position.c + radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	r1 = position.r - radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	r2 = position.r + radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	for (int i = r1; i <= r2; i++)
	{
		for (int j = c1; j <= c2; j++)
		{
			arr.insert(pair<Greed::coords, int>(Greed::coords(i, j), 1));
		}
	}
	vector<Greed::coords> l;
	for (auto c : arr)
	{
		l.push_back(c.first);
	}
	return l;
}
vector<int> ship::shipsInMyRadius()//returns the list of ships currently in my radius
{
	//which side the user wants to check
	//here front and rear are of the calling ship
	vector<int> l;//the list to be returned
	for (int i = 0; i < shipInfoList.size(); i++)
	{
		if (shipInfoList[i].getShipId() != this->ship_id && shipInfoList[i].ob->died == 0)
		{
			bool front = isShipInMyRadius(shipInfoList[i].getShipId(), ShipSide::FRONT);
			bool rear = isShipInMyRadius(shipInfoList[i].getShipId(), ShipSide::REAR);
			if (front || rear)
			{
				l.push_back(shipInfoList[i].getShipId());
			}

		}

	}
	return l;
}

vector<int> ship::shipsIamInRadiusOf()
{
	vector<int> l;
	for (int i = 0; i < shipInfoList.size(); i++)
	{
		if (shipInfoList[i].getShipId() != this->ship_id && shipInfoList[i].ob->died == 0)
		{
			bool front, rear;//opponent's 
			//for FRONT side
			front = isInShipRadius(shipInfoList[i].getShipId(), getCurrentTile(), ShipSide::FRONT);
			rear = isInShipRadius(shipInfoList[i].getShipId(), getCurrentTile(), ShipSide::REAR);
			bool Front = false;
			bool Rear = false;
			if (front || rear)
			{

				Front = true;
			}

			//for Rear
			front = isInShipRadius(shipInfoList[i].getShipId(), getCurrentRearTile(), ShipSide::FRONT);
			rear = isInShipRadius(shipInfoList[i].getShipId(), getCurrentRearTile(), ShipSide::REAR);
			if (front || rear)
			{
				Rear = true;
			}
			if (Front || Rear)
			{
				l.push_back(shipInfoList[i].getShipId());
			}
		}
	}
	return l;
}

double ship::getDistance(int s_id)//returns the eucledian distance between two ships using the tile_pos
{
	//just like finding the heuristic this is not the exact distance:
	//this is just an approximation of the original distance, original distance might vary
	Greed::coords c1 = shipInfoList[s_id].getCurrentTile();//getting the position of the ship in question
	Greed::coords c2 = getCurrentTile();
	return sqrt(pow((c1.r - c2.r), 2) + pow((c1.c - c2.c), 2));

}



void ship::update_tile_pos(double x, double y)//this function is called under the mutex m
{

	//getting the current+1 position from the list
	Direction d = Direction::NA;

	absolutePosition.x = x;
	absolutePosition.y = y;

	if (path.howMany() - 2 >= pointPath + 1 && tile_path.howMany() - 1 >= pointTilePath + 1)
	{

		Greed::abs_pos next = path[pointPath + 1];

		Greed::abs_pos current = path[pointPath];
		x = current.x;
		y = current.y;
		Greed::coords final;
		Greed::coords final1;//for the tile_pos_rear
		Greed::coords current_tile = tile_path[pointTilePath];



		if (current.y - next.y == 1 && current.x == next.x)//direction is NORTh
		{
			d = Direction::NORTH;
		}
		else if (next.x - current.x == 1 && current.y == next.y)// direction is East
		{
			d = Direction::EAST;

		}
		else if (current.x - next.x == 1 && current.y == next.y)// direction is West
		{
			d = Direction::WEST;
		}
		else if (next.y - current.y == 1 && current.x == next.x)//direction is SOUTH
		{
			d = Direction::SOUTH;
		}
		else if (current.y - next.y == 1 && next.x - current.x == 1)//for north east
		{
			d = Direction::NORTH_EAST;
		}
		else if (next.x - current.x == 1 && next.y - current.y == 1)//for south east
		{
			d = Direction::SOUTH_EAST;
		}
		else if (current.x - next.x == 1 && current.y - next.y == 1)//for north west
		{
			d = Direction::NORTH_WEST;
		}
		else if (next.y - current.y == 1 && current.x - next.x == 1)//for south west
		{
			d = Direction::SOUTH_WEST;
		}
		//setting the tile_pos according to the direction 
		else
		{
			d = Direction::NA;
		}
		dir = d;
		//updating tile_pos_front here
		if (d == Direction::NORTH || d == Direction::WEST || d == Direction::NORTH_WEST)//check for top left corner of the sprite i.e x and y
		{

			final.r = (int)current.y / len;
			final.c = (int)current.x / len;
			front_abs_pos.x = current.x;
			front_abs_pos.y = current.y;

		}
		else if (d == Direction::EAST || d == Direction::NORTH_EAST)//check with top right corner of the sprite.
		{


			if ((int)(current.x + len) % len == 0 && (int)current.y % len == 0)
			{
				final = current_tile;

			}
			else
			{
				final.r = (int)current.y / len;
				final.c = (int)(current.x + len) / len;
			}
			front_abs_pos.x = (current.x + len);
			front_abs_pos.y = current.y;

		}
		else if (d == Direction::SOUTH || d == Direction::SOUTH_WEST)//using the bottom left corner
		{
			if ((int)(current.x) % len == 0 && (int)(current.y + len) % len == 0)
			{
				final = current_tile;
			}
			else
			{
				final.r = (int)(current.y + len) / len;

				final.c = (int)current.x / len;
			}
			front_abs_pos.x = current.x;
			front_abs_pos.y = current.y + len;
		}
		else if (d == Direction::SOUTH_EAST)
		{
			if ((int)(current.x + len) % len == 0 && (int)(current.y + len) % len == 0)
			{
				final = current_tile;
			}
			else
			{
				final.r = (int)(current.y + len) / len;
				final.c = (int)(current.x + len) / len;
			}
			front_abs_pos.x = (current.x + len);
			front_abs_pos.y = (current.y + len);
		}
		//updating tile_pos_rear

		if (d == Direction::SOUTH || d == Direction::SOUTH_EAST || d == Direction::EAST)//cheking for top left corner
		{

			final1.r = (int)current.y / len;
			final1.c = (int)current.x / len;
			rear_abs_pos.x = current.x;
			rear_abs_pos.y = current.y;

		}
		else if (d == Direction::NORTH || d == Direction::NORTH_EAST)//checking for the bottom left corner
		{
			if ((int)current.x % len == 0 && (int)(current.y + len) % len == 0)
			{
				final1 = current_tile;
			}
			else
			{
				final1.c = (int)x / len;
				final1.r = (int)(y + len) / len;
			}
			rear_abs_pos.x = x;
			rear_abs_pos.y = y + len;
		}
		else if (d == Direction::SOUTH_WEST || d == Direction::WEST)//top right corner
		{
			if ((int)(x + len) % len == 0 && (int)y % len == 0)
			{
				final1 = current_tile;
			}
			else
			{
				final1.c = (int)(x + len) / len;
				final1.r = (int)(y) / len;
			}
			rear_abs_pos.x = x + len;
			rear_abs_pos.y = y;
		}
		else if (d == Direction::NORTH_WEST)//checking for the down right corner
		{
			if ((int)(x + len) % len == 0 && (int)(y + len) % len == 0)
			{
				final1 = current_tile;
			}
			else
			{
				final1.c = (int)(x + len) / len;
				final1.r = (int)(y + len) / len;
			}
			rear_abs_pos.x = x + len;
			rear_abs_pos.y = y + len;
		}


		//updating the pointer

		int ret = getIndex(tile_path, final);
		if (ret == -1)
		{
			cout << "\n----------------------------------------";
			cout << "\n logs of if part";
			cout << "\n error: getIndex of update_tile_pos returned -1";
			cout << "\n the value of final is==>" << final.r << " " << final.c;
			cout << "\n at the position==>" << x << " " << y;
			cout << "\n the previous position is==>" << path[pointPath - 1].x << " " << path[pointPath - 1].y;
			cout << "\n the direction is==>" << (int)d;
			cout << "\n value of pointPath==>" << pointPath;
			cout << "\n total in path list is==>" << path.howMany();
			cout << "\n-------------------------------------";
		}
		else
		{
			pointTilePath = ret;
		}
		tile_pos_front = final;
		tile_pos_rear = final1;





	}
	else
	{
		Greed::coords final1;//for the tile_pos_rear
		Greed::coords current_tile = tile_path[pointTilePath];
		Direction d = dir;
		//for updating front_abs_pos
		if (d == Direction::NORTH || d == Direction::WEST || d == Direction::NORTH_WEST)//check for top left corner of the sprite i.e x and y
		{


			front_abs_pos.x = x;
			front_abs_pos.y = y;

		}
		else if (d == Direction::EAST || d == Direction::NORTH_EAST)//check with top right corner of the sprite.
		{



			front_abs_pos.x = (x + len);
			front_abs_pos.y = y;

		}
		else if (d == Direction::SOUTH || d == Direction::SOUTH_WEST)//using the bottom left corner
		{

			front_abs_pos.x = x;
			front_abs_pos.y = y + len;
		}
		else if (d == Direction::SOUTH_EAST)
		{

			front_abs_pos.x = (x + len);
			front_abs_pos.y = (y + len);
		}
		//for updating tile pos
		if (d == Direction::SOUTH || d == Direction::SOUTH_EAST || d == Direction::EAST)//cheking for top left corner
		{

			final1.r = (int)y / len;
			final1.c = (int)x / len;

			rear_abs_pos.x = x;
			rear_abs_pos.y = y;

		}
		else if (d == Direction::NORTH || d == Direction::NORTH_EAST)//checking for the bottom left corner
		{
			if ((int)x % len == 0 && (int)(y + len) % len == 0)
			{
				final1 = current_tile;
			}
			else
			{
				final1.c = (int)x / len;
				final1.r = (int)(y + len) / len;
			}
			rear_abs_pos.x = x;
			rear_abs_pos.y = y + len;
		}
		else if (d == Direction::SOUTH_WEST || d == Direction::WEST)//top right corner
		{
			if ((int)(x + len) % len == 0 && (int)y % len == 0)
			{
				final1 = current_tile;
			}
			else
			{
				final1.c = (int)(x + len) / len;
				final1.r = (int)(y) / len;
			}
			rear_abs_pos.x = x + len;
			rear_abs_pos.y = y;
		}
		else if (d == Direction::NORTH_WEST)//checking for the down right corner
		{
			if ((int)(x + len) % len == 0 && (int)(y + len) % len == 0)
			{
				final1 = current_tile;
			}
			else
			{
				final1.c = (int)(x + len) / len;
				final1.r = (int)(y + len) / len;
			}
			rear_abs_pos.x = x + len;
			rear_abs_pos.y = y + len;
		}


		tile_pos_front = current_tile;
		tile_pos_rear = final1;

		int ret = getIndex(tile_path, current_tile);
		if (ret == -1)
		{
			cout << "\n----------------------------------------";
			cout << "\n logs of else part";
			cout << "\n error: getIndex of update_tile_pos returned -1";
			cout << "\n the value of final is==>" << current_tile.r << " " << current_tile.c;
			cout << "\n at the position==>" << x << " " << y;
			cout << "\n the previous position is==>" << path[pointPath - 1].x << " " << path[pointPath - 1].y;
			cout << "\n the direction is==>" << (int)d;
			cout << "\n value of pointPath==>" << pointPath;
			cout << "\n-------------------------------------";
		}
		else
		{
			pointTilePath = ret;
		}

	}



}
bool ship::getNextCurrentEvent(Event& ob)
{
	unique_lock<mutex> lk(mutx->event_mutex[ship_id]);
	if (current_event.size() > 0)
	{
		ob = current_event[0];
		current_event.pop_front();
		return true;
	}
	return false;



}
int getIndex(List<Greed::abs_pos> ob, Greed::abs_pos val)
{
	int index = -1;
	for (int i = 0; i < ob.howMany(); i++)
	{
		if (ob[i].x == val.x && ob[i].y == val.y)
		{
			index = i;
		}
	}
	return index;
}

int aux1(List<Greed::cannon>& cannon, Greed::coords ob)
{
	int index = -1;
	for (int i = 0; i < cannon.howMany(); i++)
	{
		if (cannon[i].getCannonTile() == ob)
		{
			index = i;
			break;
		}
	}
	return index;
}

void filter(List<Greed::coords>& ob, List<Greed::coords> opaque_coords, ship sob)//function for removing the coords which are out of range
{
	/*
	* here range is the number of columns-1 and number of rows-1 and row and column must be greater than 0
	*/

	for (int i = 0; i < ob.howMany(); i++)
	{
		if (ob[i].r >= 0 && ob[i].r < rows && ob[i].c >= 0 && ob[i].c < columns)
		{
			continue;
		}
		else
		{
			ob.delAt(i);
			i--;
		}
	}

	List<Greed::coords> temp;
	int found = 0;

	temp.add_rear(ob[0]);

	/*
	for (int i = 0; i < opaque_coords.howMany(); i++)
	{
		cout << opaque_coords[i].r << " " << opaque_coords[i].c << endl;
	}
	*/
	for (int i = 0; i < ob.howMany() - 1; i++)//break the loop when we find the first obstacle
	{
		//cout << "\n for=>" << ob[i].r << " " << ob[i].c;
		if (sob.getShipDirection() == Direction::NORTH_EAST)
		{

			int found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r - 1, ob[i + 1].c));
			if (found != -1)
				break;

			found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r, ob[i].c + 1));
			if (found != -1)
				break;
			if (getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i + 1].r, ob[i + 1].c)) == -1)
				temp.add_rear(ob[i + 1]);
			else
				break;
		}
		else if (sob.getShipDirection() == Direction::NORTH_WEST)
		{

			int found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r - 1, ob[i].c));//checking for south
			if (found != -1)
				break;
			//checking for east
			found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r, ob[i].c - 1));
			if (found != -1)
				break;
			if (getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i + 1].r, ob[i + 1].c)) == -1)
				temp.add_rear(ob[i + 1]);
			else
				break;
		}

		else if (sob.getShipDirection() == Direction::SOUTH_WEST)
		{
			int found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r + 1, ob[i].c));
			if (found != -1)
				break;

			found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r, ob[i].c - 1));
			if (found != -1)
				break;
			// cout << "\n checking for==>" << ob[i+1].r << " " << ob[i+1].c;
			if (getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i + 1].r, ob[i + 1].c)) == -1)
				temp.add_rear(ob[i + 1]);
			else
				break;
		}
		else if (sob.getShipDirection() == Direction::SOUTH_EAST)
		{
			int found = getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i].r + 1, ob[i].c));//checking for south
			if (found != -1)
				break;
			//checking for east
			found = getIndex <Greed::coords>(opaque_coords, Greed::coords(ob[i].r, ob[i].c + 1));
			if (found != -1)
				break;
			if (getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i + 1].r, ob[i + 1].c)) == -1)
				temp.add_rear(ob[i + 1]);
			else
				break;
		}
		else//for north south east and west
		{
			int found = getIndex<Greed::coords>(opaque_coords, ob[i]);
			if (found != -1)
			{
				break;
			}
			if (getIndex < Greed::coords>(opaque_coords, Greed::coords(ob[i + 1].r, ob[i + 1].c)) == -1)
				temp.add_rear(ob[i + 1]);
			else
				break;
		}
	}

	ob.erase();
	cout << "\n elements in temp is==>";
	for (int i = 0; i < temp.howMany(); i++)
	{
		cout << temp[i].r << " " << temp[i].c << endl;
	}
	ob = temp;

}
void chaseShip1(int s_id, ship& ob)//the famous chasing  a ship function
{

	ob.mutx->mchase[ob.ship_id].lock();
	ob.autopilot = 1;
	ob.mutx->mchase[ob.ship_id].unlock();

	deque<shipInfo> l = ob.getShipList();
	Greed::path_attribute path;
	path = ob.setTarget(l[s_id].getCurrentTile());
	ob.setPath(path.target);
	Greed::coords temp = l[s_id].getCurrentTile();
	while (1)
	{
		if (ob.mutx->mchase[ob.ship_id].try_lock())
		{
			if (!ob.isInShipRadius(s_id, temp) && ob.autopilot == 1)//not in the ships radius now
			{

				path = ob.setTarget(l[s_id].getCurrentTile());
				ob.setPath(path.target);
				temp = l[s_id].getCurrentTile();
				//cout << "\nlap at==>" << ob.getCurrentTile().r << " " << ob.getCurrentTile().c;
			}
			else if (ob.autopilot == 0)
			{
				ob.mutx->mchase[ob.ship_id].unlock();
				break;
			}
			ob.mutx->mchase[ob.ship_id].unlock();
		}
		Sleep(100);
	}




}
bool ship::setPath(List<Greed::coords> ob, int state)  //for adding the supplied list to  the Linked list of the user movement
{
	/*if state is:
	* 1-> it means a destination is set
	* 0-> no destination is set and the ship wants to anchor
	*/
	//locking the mutex first

	if (path.howMany() > 0)
	{

		mutx->m[mutex_id].lock();
		//setting the tile_path list here
		if (state == 1)
		{
			int temp = tile_path.howMany();
			for (int i = 0; i < temp; i++)
			{
				tile_path.del_front();
			}

			tile_path = ob;
			pointTilePath = 0;
		}

		if ((int)path[pointPath].x % len == 0 && (int)path[pointPath].y % breadth == 0)
		{
			//deleting the elements from the current list

			int temp = path.howMany();
			for (int i = 0; i < temp; i++)
			{
				path.del_rear();
			}

			if (state == 1)
			{
				path = map_ob.makeList(ob);//the list of the path is set here

			}
			pointPath = 0;


			mutx->m[mutex_id].unlock();

		}
		else
		{
			//setting x and y to the value till the list has to be kept intact


			double x = tile_pos_front.c * len;
			double y = tile_pos_front.r * breadth;




			int k = 0;
			while (k != pointPath)
			{
				path.del_front();
				k++;
			}
			int index = ::getIndex(path, Greed::abs_pos(x, y));
			index++;

			//deleting from index to end




			int temp = path.howMany();
			for (int i = index; i < temp; i++)
			{

				path.delAt(index);
			}
			if (state == 1)
			{
				List<Greed::abs_pos> l1;
				l1 = map_ob.makeList(ob);

				for (int i = 0; i < l1.howMany(); i++)
				{
					path.add_rear(l1[i]);
				}
			}
			pointPath = 0;
			map_ob.deleteDuplicate(path);
			mutx->m[mutex_id].unlock();


		}
		return true;
	}
	else//if the path list is empty
	{
		if (state == 1)
		{
			mutx->m[mutex_id].lock();
			//for the tile_path list
			int temp = tile_path.howMany();
			for (int i = 0; i < temp; i++)
			{
				tile_path.del_front();
			}

			pointTilePath = 0;

			tile_path = ob;
			//for the abs_pos list
			path = map_ob.makeList(ob);


			mutx->m[mutex_id].unlock();
			return true;
		}
		pointPath = 0;
	}
	return false;

}

/*
attribute ship::whatsHere(Map::coords ob)//pass the row and column of the tile to get whats there
{
	int index = ob.r * columns + ob.c;
	attribute obb;
	mutx->m_global_map.lock();
	if (int(globalMap[index].b.tile_id) == 0)// water
	{
		obb.update(Entity::WATER, WATER_COST, -1, -1, Bonus::NA, 2369);
	}
	else if (int(globalMap[index].b.tile_id) == 1)//storm
	{
		obb.update(Entity::STORM, STORM_COST, -1, -1, Bonus::NA, 2369);
	}
	else if (int(globalMap[index].b.tile_id) == 2)//land
	{
		obb.update(Entity::LAND, -1, -1, -1, Bonus::NA, 2369);
	}
	else if (int(globalMap[index].b.tile_id) == 3)//CANNON
	{
		obb.update(Entity::CANNON, -1, -1, globalMap[index].b.cannon_id, Bonus::NA, 2369);
	}
	else if (int(globalMap[index].b.tile_id) == 4)//Bonus
	{
		obb.update(Entity::BONUS, globalMap[index].b.cost, -1, -1, globalMap[index].b.bonus, 2369);
	}
	//checking for the ship
	mutx->m_global_map.unlock();
	Control ob1;

	List<ship*> l1;
	l1 = ob1.getShipList(2369);
	for (int i = 0; i < l1.howMany(); i++)
	{
		if (ob == l1[i]->getCurrentTile())
		{
			obb.updateShipStatus(l1[i]->ship_id, 2369);
		}
	}


	return obb;
}
*/
attribute ship::whatsHere(Greed::coords ob, int m)//pass the row and column of the tile to get whats there
{
	/*
	* if m is 0 then global map is looked into for the attribute information
	* if m is 1 then local map is looked into for attribute information
	*/
	// cout << "\n inside the function==>" << localGreed[10 * columns + 0].b.cost;
	int index = ob.r * columns + ob.c;//this is the index of ob in the graph
	List<Greed::vertex> graph;
	if (m == 0)
	{
		graph = getGlobalMapref(2369);
	}
	else if (m == 1)//call the localMap
	{
		//
		graph = localMap;

	}
	attribute att;
	Control cont;

	List<Greed::cannon> cannon = cont.getCannonList(2369);
	deque<ship*> sh = cont.getShipList(2369);

	//checking for the water first if its a water then checking for the storm
	if (graph[index].b.tile_id == Entity::WATER)
	{


		att.cost = WATER_COST;


	}
	//checking if its land then it can be simple land,cannon, or bonus
	else if (graph[index].b.tile_id == Entity::LAND)
	{

		if (aux1(cannon, ob) != -1)
		{


			att.cannon_id = aux1(cannon, ob);
			att.cost = -1;

		}
		else
		{

			att.cost = -1;
		}
	}
	//checking for the ship
	int ind = -1;
	for (int i = 0; i < sh.size(); i++)
	{
		if (sh[i]->tile_pos_front == ob)
		{
			ind = i;
			break;
		}
	}
	if (ind != -1)
	{

		att.ship_id = ind;
	}
	if (m == 1)
	{
		att.cost = graph[index].b.cost;
	}
	return att;

}

bool ship::sail(Direction d, int tiles = 1)//number of tiles to be moved at a particular time
{
	//here tile_path list is not associated with the tile_path variable in the ship class.
	//no need to lock any mutex here
	dir = d;
	if (d != Direction::NA && tiles >= 1)
	{
		//deleting the old coords path
		List<Greed::coords> tile_path;
		tile_path.add_rear(tile_pos_front);
		if (d == Direction::NORTH)
		{

			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r - i, tile_pos_front.c));
			}
		}
		else if (d == Direction::EAST)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r, tile_pos_front.c + i));
			}
		}
		else if (d == Direction::WEST)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r, tile_pos_front.c - i));
			}
		}
		else if (d == Direction::SOUTH)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r + i, tile_pos_front.c));
			}
		}
		else if (d == Direction::NORTH_EAST)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r - i, tile_pos_front.c + i));
			}
		}
		else if (d == Direction::NORTH_WEST)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r - i, tile_pos_front.c - i));
			}
		}
		else if (d == Direction::SOUTH_EAST)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r + i, tile_pos_front.c + i));
			}
		}
		else if (d == Direction::SOUTH_WEST)
		{
			for (int i = 1; i <= tiles; i++)
			{
				tile_path.add_rear(Greed::coords(tile_pos_front.r + i, tile_pos_front.c - i));
			}
		}
		//sending the list to the setPath funtion
		/*
		* debugging info

		cout << "\n returned tilePath is==>";
		for (int i = 0; i < tile_path.howMany(); i++)
		{
			cout << tile_path[i].r << " " << tile_path[i].c << endl;
		}
		*/
		Control con;
		::filter(tile_path, con.opaque_coords, *this);//function to filter out of range coordinates
		/*
		cout << "\n tile path after filtering is==>";
		for (int i = 0; i < tile_path.howMany(); i++)
		{
			cout << tile_path[i].r << " " << tile_path[i].c << endl;
		}
		*/

		setPath(tile_path);
		return true;
	}
	return false;
}

Greed::path_attribute ship::setTarget(Greed::coords ob)
{
	//this function will return the path_attribute object containing information about the exact path to take
	Greed::path_attribute ob1;//to be returned
	mutx->m[mutex_id].lock();
	mutx->m_global_map.lock();
	map_ob.path(tile_pos_front, ob, localMap, ob1, localMap);

	mutx->m_global_map.unlock();
	mutx->m[mutex_id].unlock();
	ob1.target.add_front(tile_pos_front);// starting from the current position

	return ob1;

}

Greed::path_attribute ship::setTarget(int s_id)
{
	Greed::path_attribute ob1;//to be  returned
	//finding the current coordinate of the ship given in the object

	Control ob2;
	deque<ship*> ship_list;
	ship_list = ob2.getShipList(2369);
	int index = s_id;

	if (index != -1)
	{
		Greed::coords tar = ship_list[index]->getCurrentTile();
		return setTarget(ship_list[index]->getCurrentTile());


	}
	else
	{
		return ob1;
	}
}

bool ship::updateCost(Greed::coords ob, double new_cost)//update the cost of the local map // no need for locking the mutex
{
	if (new_cost >= 0 && map_ob.isValidCoords(ob))
	{
		// int ind=map_ob.Map::find_index(ob,ship::localMap);//index of the coord given for the cost to be updated
		int ind = ob.r * columns + ob.c;
		ship::localMap[ind].b.cost = new_cost;
		return true;
	}
	return false;
}

bool ship::updateCost(Entity e, double new_cost, Bonus  b = Bonus::NA)
{

	Control ob;
	List < Greed::coords> storm = ob.getStormList();
	List<Greed::coords> bonus = ob.getBonusList();

	if (e == Entity::NA || new_cost < 0)
	{
		return false;
	}
	if (e == Entity::WATER)
	{
		//traversing the entire local graph
		for (int i = 0; i < localMap.howMany(); i++)
		{
			if (localMap[i].b.tile_id == Entity::WATER)
			{
				localMap[i].b.cost = new_cost;
			}
		}
		return true;
	}
	else if (e == Entity::STORM)
	{
		//traversing the entire local graph
		for (int i = 0; i < storm.howMany(); i++)
		{
			for (int j = 0; j < localMap.howMany(); j++)
			{
				if (storm[i] == localMap[j].b)
				{
					localMap[j].b.cost = new_cost;
				}
			}
		}
		return true;
	}
	else if (e == Entity::BONUS && b != Bonus::NA)
	{
		for (int i = 0; i < bonus.howMany(); i++)
		{
			if (bonus[i].bonus == b)
			{
				for (int j = 0; j < localMap.howMany(); j++)
				{
					if (bonus[i] == localMap[j].b)
					{
						localMap[j].b.cost = new_cost;
					}
				}
			}
		}
	}

}

bool ship::isInShipRadius(int s_id, Greed::coords ob, ShipSide opponent_side)//s_id is the id of the id whose radius has to be checked
{

	Control obb;
	deque<ship*> l1;
	l1 = obb.getShipList(2369);
	if (l1[s_id]->getDiedStatus() == 1)
	{
		return false;
	}
	int radius = l1[s_id]->getShipRadius();
	Greed::coords position;
	if (opponent_side == ShipSide::FRONT)
	{
		position = l1[s_id]->getCurrentTile();
	}
	else if (opponent_side == ShipSide::REAR)
		position = l1[s_id]->getCurrentRearTile();

	int c1 = position.c - radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int c2 = position.c + radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int r1 = position.r - radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	int r2 = position.r + radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	//checking if its in the radius or not;
	if (ob.c >= c1 && ob.c <= c2 && ob.r >= r1 && ob.r <= r2)
	{
		return true;
	}
	else
		return false;
}
bool ship::isInShipRadius(int s_id, ShipSide myside, ShipSide opponent_side)
{
	if (shipInfoList[s_id].getDiedStatus() == 1)
	{
		return false;
	}
	for (int i = 0; i < shipInfoList.size(); i++)
	{
		if (shipInfoList[i].getShipId() != this->ship_id)
		{
			if (myside == ShipSide::FRONT)
			{
				return isInShipRadius(s_id, getCurrentTile(), opponent_side);
			}
			else if (myside == ShipSide::REAR)
			{
				return isInShipRadius(s_id, getCurrentRearTile(), opponent_side);
			}
		}
	}
}
bool ship::isCannonInRadius(int c_id, ShipSide side)
{
	if (cannon_list[c_id].isDead == true)
	{
		return false;
	}
	return isInShipRadius(this->ship_id, cannon_list[c_id].getCannonTile(), side);
}
vector<int> ship::cannonsInMyRadius()
{

	Control ob;
	List<Greed::cannon> cannon_list = ob.getCannonList(2369);
	vector<int> l;
	for (int i = 0; i < cannon_list.howMany(); i++)
	{
		if (cannon_list[i].isDead == true)
		{
			continue;
		}
		bool front = false;
		bool rear = false;
		front = isCannonInRadius(cannon_list[i].cannon_id, ShipSide::FRONT);
		rear = isCannonInRadius(cannon_list[i].cannon_id, ShipSide::REAR);
		if (front || rear)
		{
			l.push_back(cannon_list[i].cannon_id);

		}
	}
	return l;

}
bool ship::isShipInMyRadius_forFire(int s_id, cannon myside, ShipSide oppSide)//to check if s_id is in the callers ship radius
{
	/*documentation
	* myside is the side of your ship which will be used as a refrence to check
	* opponent_side is the side of the opponents ship which will be checked
	*/
	//just find the current cooordinate of the ship id passed in the arguments
	Control ob;
	deque<ship*> l;
	l = ob.getShipList(2369);
	if (s_id <= l.size() - 1 && s_id >= 0 && l[s_id]->getDiedStatus() == 0)
	{
		Greed::coords pos;
		//chekcing for front side of opponent first
		if (oppSide == ShipSide::FRONT)
		{
			Greed::coords pos = l[s_id]->getCurrentTile();
			bool check = isInShipRadius(this->ship_id, pos, (ShipSide)(int)myside);
			if (check)
			{
				return true;
			}
			else
				return false;
		}
		else if (oppSide == ShipSide::REAR)
		{
			Greed::coords pos = l[s_id]->getCurrentRearTile();
			bool check = isInShipRadius(this->ship_id, pos, (ShipSide)(int)myside);
			if (check)
			{
				return true;
			}
			else
				return false;
		}
	}
}
bool ship::isShipInMyRadius(int s_id, ShipSide oppSide)//to check if s_id is in the callers ship radius
{
	//this works as follows
	/*
	* this checks if oppSide(side of the opponent) is in the radius of current ship irrespective of its cannon position(both for front and rear)
	*/
	Control ob;
	deque<ship*> l;
	l = ob.getShipList(2369);
	if (s_id <= l.size() - 1 && s_id >= 0 && l[s_id]->getDiedStatus() == 0)
	{
		Greed::coords pos;
		//chekcing for front side of opponent first
		if (oppSide == ShipSide::FRONT)
		{
			Greed::coords pos = l[s_id]->getCurrentTile();
			bool check = isInShipRadius(this->ship_id, pos, ShipSide::FRONT);
			bool check1 = isInShipRadius(this->ship_id, pos, ShipSide::REAR);
			if (check || check1)
			{
				return true;
			}
			else
				return false;
		}
		else if (oppSide == ShipSide::REAR)
		{
			Greed::coords pos = l[s_id]->getCurrentRearTile();
			bool check = isInShipRadius(this->ship_id, pos, ShipSide::FRONT);
			bool check1 = isInShipRadius(this->ship_id, pos, ShipSide::REAR);
			if (check || check1)
			{
				return true;
			}
			else
				return false;
		}



	}

}
bool ship::chaseShip(int s_id)
{

	if (autopilot == 0)
	{

		thread t(chaseShip1, s_id, ref(*this));
		t.detach();


	}
	Sleep(300);
	return true;
}
//make a filter function to remove the out of bound coords from the trajectory list given
void ship::setBullet_forCannon(Greed::bullet& bull)
{
	cannon can = bull.can;
	int c_id = bull.target_cannon;
	if (can == cannon::FRONT)
	{
		bull.starting_tile = this->tile_pos_front;
	}
	else if (can == cannon::REAR)
	{
		bull.starting_tile = this->tile_pos_rear;
	}
	bull.ending_tile = cannon_list[c_id].getCannonTile();
	Direction launch = this->dir;
	Greed::abs_pos launch_coords;
	Greed::abs_pos target_coords;

	if (launch == Direction::EAST || launch == Direction::WEST)
	{
		launch_coords = Greed::abs_pos(getAbsolutePosition1(ShipSide((int)can)).x, getAbsolutePosition1(ShipSide((int)can)).y + (len / 2));
	}
	else if (launch == Direction::NORTH || launch == Direction::SOUTH)
	{
		launch_coords = Greed::abs_pos(getAbsolutePosition1(ShipSide((int)can)).x + (len / 2), getAbsolutePosition1(ShipSide((int)can)).y);
	}
	else//assuming that by default the direction is set as NORTH
	{
		launch_coords = Greed::abs_pos(getAbsolutePosition1(ShipSide((int)can)).x, getAbsolutePosition1(ShipSide((int)can)).y);
	}
	target_coords = Greed::abs_pos(bull.ending_tile.c * 80 + (len / 2), bull.ending_tile.r * 80 + (len / 2));

	List<Greed::abs_pos> tempo = map_ob.makeTrajectory(launch_coords.x, launch_coords.y, target_coords.x, target_coords.y);
	for (int i = 0; i < tempo.howMany(); i++)
	{
		bull.bullet_trajectory.push_back(tempo[i]);
	}
	// bull.starting_tile = Greed::coords((int)launch_coords.y / 80, (int)launch_coords.x / 80);

	//cannon_ob.activeBullets.add_rear(bull);
	boundingEntity radientity = getRadiBound(bull.starting_tile);//for cutting off extra coordinates that are outside the boundary of the firing ship
	ship::filter(bull.bullet_trajectory, radientity);//filter function for cutting off extra coordinates
	bull.bullet_trajectory.pop_back();
	this->ammo--;
}
bool ship::fireAtCannon(int c_id, cannon can = cannon::FRONT)
{
	if (cannon_ob.activeBullets.size() <= 1 && isCannonInRadius(c_id, (ShipSide)(int)can) && ammo > 0 && cannon_list[c_id].isDead == false)//if cannon is in the provided radius of the firing ship
	{

		Control ob;

		Greed::bullet bull;
		bull.initialize(true, mutx, cannon_ob.activeBullets.size(), this->ship_id, cannon_ob.power, -1, this->ship_id, c_id, -1, true, can, ShipSide::NA);


		mutx->m[ship_id].lock();
		//cannon_ob.allBullets.add_rear(bull);

		// cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;
		cannon_ob.activeBullets.push_back(bull);//only the active bullets are kept here
		//cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;

		mutx->m[ship_id].unlock();

	}
	return true;
}
void ship::setBullet(Greed::bullet& bull, cannon can, int s_id, ShipSide s)
{
	if (can == cannon::FRONT)
	{
		bull.starting_tile = this->tile_pos_front;
	}
	else if (can == cannon::REAR)
	{
		bull.starting_tile = this->tile_pos_rear;
	}
	// cout << "\n id of the firing ship is==>" << this->ship_id;
	 //setting the ending tile
	 //cout << "\n in func starting tile=>" << bull.starting_tile.r << " " << bull.starting_tile.c;
	if (s == ShipSide::MIDDLE || s == ShipSide::FRONT)
	{
		bull.ending_tile = shipInfoList[s_id].getCurrentTile();
	}
	else if (s == ShipSide::REAR)
	{
		bull.ending_tile = shipInfoList[s_id].getCurrentRearTile();
	}

	//checking from where the bullet will be launched
	Direction launch = this->dir;//this locks the internal mutex
	Direction target = shipInfoList[s_id].getShipDirection();
	Greed::abs_pos launch_coords;
	Greed::abs_pos target_coords;
	if (launch == Direction::EAST || launch == Direction::WEST)
	{
		launch_coords = Greed::abs_pos(getAbsolutePosition1(ShipSide((int)can)).x, getAbsolutePosition1(ShipSide((int)can)).y + (len / 2));
	}
	else if (launch == Direction::NORTH || launch == Direction::SOUTH)
	{
		launch_coords = Greed::abs_pos(getAbsolutePosition1(ShipSide((int)can)).x + (len / 2), getAbsolutePosition1(ShipSide((int)can)).y);
	}
	else//assuming that by default the direction is set as NORTH
	{
		launch_coords = Greed::abs_pos(getAbsolutePosition1(ShipSide((int)can)).x, getAbsolutePosition1(ShipSide((int)can)).y);
	}
	//checking for the target_coords now
	shipEntity entity = shipInfoList[s_id].ob->getShipEntity();//eror here target ship's entity has to be found
	if (target == Direction::EAST)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c2.x - (len / 4), entity.c2.y + (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 4), entity.c1.y + (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}
	}
	else if (target == Direction::WEST)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 4), entity.c1.y + (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c2.x - (len / 4), entity.c2.y + (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}
	}
	else if (target == Direction::NORTH)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c4.x + (len / 2), entity.c4.y - (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}
	}
	else if (target == Direction::SOUTH)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c4.x + (len / 2), entity.c4.y - (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}
	}
	else if (target == Direction::NORTH_EAST)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c2.x - (len / 4), entity.c2.y + (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c4.x + (len / 4), entity.c4.y - (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}

	}
	else if (target == Direction::NORTH_WEST)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 4), entity.c1.y + (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c3.x - (len / 4), entity.c3.y - (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}

	}
	else if (target == Direction::SOUTH_EAST)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c3.x - (len / 4), entity.c3.y - (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 4), entity.c1.y + (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}

	}
	else if (target == Direction::SOUTH_WEST)
	{
		if (s == ShipSide::FRONT)
		{
			target_coords = Greed::abs_pos(entity.c4.x + (len / 4), entity.c4.y - (len / 4));
		}
		else if (s == ShipSide::REAR)
		{
			target_coords = Greed::abs_pos(entity.c2.x - (len / 4), entity.c2.y + (len / 4));
		}
		else if (s == ShipSide::MIDDLE)
		{
			target_coords = Greed::abs_pos(entity.c1.x + (len / 2), entity.c1.y + (len / 2));
		}

	}

	//setting the trajectory of the bullet

	List<Greed::abs_pos> temp = map_ob.makeTrajectory(launch_coords.x, launch_coords.y, target_coords.x, target_coords.y);
	for (int i = 0; i < temp.howMany(); i++)
	{
		bull.bullet_trajectory.push_back(temp[i]);
	}
	// bull.starting_tile = Greed::coords((int)launch_coords.y / 80, (int)launch_coords.x / 80);

	 //cannon_ob.activeBullets.add_rear(bull);
	boundingEntity radientity = getRadiBound(bull.starting_tile);//for cutting off extra coordinates that are outside the boundary of the firing ship
	ship::filter(bull.bullet_trajectory, radientity);//filter function for cutting off extra coordinates
	ammo--;
	bull.bullet_trajectory.pop_back();
}
bool ship::fireCannon(cannon can, int s_id, ShipSide s)//s_id and s are of target ship
{
	/*first check:
	* if the side of the ship "s" provided is in the radius of the cannon
	* firing at the middle will only happen when both rear and front part of the ship will be in radius of the attacking ship
	*/
	//checking which part of the target ship is in the radius

	if (getCurrentAmmo() > 0 && isShipInMyRadius_forFire(s_id, can, s) && cannon_ob.activeBullets.size() <= 1)
		//if (ammo > 0 && (s == ShipSide::MIDDLE && (isShipInMyRadius(s_id, ShipSide((int)can), ShipSide::FRONT) && isShipInMyRadius(s_id, ShipSide((int)can), ShipSide::REAR))) || (s != ShipSide::MIDDLE && isShipInMyRadius(s_id, ShipSide((int)can), s)))
	{

		if (shipInfoList[s_id].ob->getDiedStatus() == 0)//fire only if the ship is still alive
		{
			mutx->m[ship_id].lock();
			Greed::bullet bull;

			bull.initialize(true, mutx, cannon_ob.activeBullets.size(), this->ship_id, cannon_ob.power, -1, this->ship_id, -1, s_id, true, can, s);
			//setting the starting and ending tile for the bullet in the game


			// cannon_ob.allBullets.push_back(bull);

			 // cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;
			cannon_ob.activeBullets.push_back(bull);//only the active bullets are kept here
			//cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;

			mutx->m[ship_id].unlock();
		}

	}



	return true;
}
bool Greed::cannon::isShipInMyRadius(int s_id, ShipSide opponent_side)
{
	if (s_id >= 0)
	{
		Greed::coords ob = tile;
		Control obb;
		deque<ship*> l1;
		l1 = obb.getShipList(2369);


		Greed::coords position;
		if (opponent_side == ShipSide::FRONT)
		{
			position = l1[s_id]->getCurrentTile();
		}
		else if (opponent_side == ShipSide::REAR)
			position = l1[s_id]->getCurrentRearTile();

		int c1 = position.c - radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int c2 = position.c + radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int r1 = position.r - radius;
		if (r1 < 0)
		{
			r1 = 0;
		}
		else if (r1 > rows)
		{
			r1 = rows - 1;
		}
		int r2 = position.r + radius;
		if (r2 < 0)
		{
			r2 = 0;
		}
		else if (r2 > rows)
		{
			r2 = rows - 1;
		}
		//checking if its in the radius or not;
		if (ob.c >= c1 && ob.c <= c2 && ob.r >= r1 && ob.r <= r2)
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
int Greed::cannon::getVictimShip()
{
	//for now the process of choosing the ship will be the  first ship that comes under the scan
	//sequential checking of the ships if they are in the radius of the cannon,
	//the cutoff will be at the first ship such found
	/*
	* this is concept1
	* this law dictates that whoever comes in my radius first will be beaten first. assume if s_id with 1 and 2 both are in the radius
	* then 1 will be beaten first.
	*/
	Control con;
	deque<ship*> ship_list = con.ship_list;
	int ship_id = current_ship;
	/*
	for (int i = 0; i < ship_list.size(); i++)
	{
		if (ship_list[i]->getCurrentHealth() > 0)
		{
			if (isShipInMyRadius(ship_list[i]->getShipId(), ShipSide::FRONT))//second parameter is for the opponent side
			{
				ship_id = ship_list[i]->getShipId();
				ss = ShipSide::FRONT;
				break;
			}
			else if (isShipInMyRadius(ship_list[i]->getShipId(), ShipSide::REAR))
			{
				ship_id = ship_list[i]->getShipId();
				ss = ShipSide::REAR;
				break;
			}
		}
	}
	current_ship = ship_id;
	return ship_id;
	*/

	/*
	* this is concept2
	* this law dictates that the ship having more health will be beaten first
	* this rule is implemented below
	*/
	int s_id;
	int maxi = INT_MIN;
	for (int i = 0; i < ship_list.size(); i++)
	{
		if (ship_list[i]->getDiedStatus() == 0 && isShipInMyRadius(ship_list[i]->getShipId(), ShipSide::FRONT))//the ship is alive
		{
			//cout << "\nfor cannon ship_id==>" << ship_list[i]->getShipId();
			if (maxi < ship_list[i]->getCurrentHealth())
			{
				ship_id = i;
				maxi = ship_list[i]->getCurrentHealth();

			}
		}
	}
	//cout << "\n ship id=>" << ship_id;
	current_ship = ship_id;
	return ship_id;

}
double Greed::cannon::get_required_angle()//getVictimSHip has to be called in order set a new ship as target
{
	Control con;
	deque<ship*> ship_list = con.ship_list;
	Greed::coords ship_tile;
	if (current_ship != -1 && ss == ShipSide::FRONT)
	{
		ship_tile = ship_list[this->current_ship]->getCurrentTile();
	}
	else if (current_ship != -1 && ss == ShipSide::REAR)
	{
		ship_tile = ship_list[this->current_ship]->getCurrentRearTile();
	}
	Greed::abs_pos sabs((ship_tile.c * len) + (len / 2), (ship_tile.r * len) + (len / 2));
	Greed::abs_pos cabs(tile.c * len + (len / 2), tile.r * len + (len / 2));
	double thetha_r;//in radians
	double thetha_d = current_angle;//in degree
	if (cabs.x - sabs.x == 0)
	{
		if (cabs.y > sabs.y)
		{
			thetha_d = 0;

			/*
			double thetha2 = 360;
			if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
			{
				thetha_d = thetha1;
			}
			else
			{
				thetha_d = thetha2;
			}
			*/
		}
		else if (cabs.y < sabs.y)
		{
			thetha_d = 180;

			/*
			double thetha2 = -180;
			if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
			{
				thetha_d = thetha1;
			}
			else
			{
				thetha_d = thetha2;
			}
			*/
		}
	}
	else
	{
		double slope = (sabs.y - cabs.y) / (cabs.x - sabs.x);
		double val = atan(slope);

		if (val < 0)
		{
			if (sabs.y < cabs.y)
			{
				thetha_r = abs(val);
				double thetha1 = ((180 / PI) * thetha_r) - 90;
				double thetha2 = 270 + ((180 / PI) * thetha_r);
				if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
				{
					thetha_d = thetha1;
				}
				else
				{
					thetha_d = thetha2;
				}
			}
			else if (sabs.y > cabs.y)
			{
				thetha_r = abs(val);
				double thetha1 = ((180 / PI) * thetha_r) + 90;
				double thetha2 = -270 + ((180 / PI) * thetha_r);
				if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
				{
					thetha_d = thetha1;
				}
				else
				{
					thetha_d = thetha2;
				}
			}
		}
		else if (val > 0)
		{
			if (sabs.y < cabs.y)
			{
				double thetha1;
				thetha_r = abs(val);
				thetha1 = 90 - ((180 / PI) * thetha_r);
				double thetha2 = -270 - ((180 / PI) * thetha_r);
				if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
				{
					thetha_d = thetha1;
				}
				else
				{
					thetha_d = thetha2;
				}
			}
			else if (sabs.y > cabs.y)
			{
				thetha_r = abs(val);

				double thetha1 = -90 - ((180 / PI) * thetha_r);
				double thetha2 = 270 - ((180 / PI) * thetha_r);
				if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
				{
					thetha_d = thetha1;
				}
				else
				{
					thetha_d = thetha2;
				}
			}
		}
		else if (val == 0)
		{
			if (sabs.x > cabs.x)
			{
				double thetha1 = 90;
				double thetha2 = -270;
				if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
				{
					thetha_d = thetha1;
				}
				else
				{
					thetha_d = thetha2;
				}

			}
			else if (sabs.x < cabs.x)
			{
				double thetha1 = -90;
				double thetha2 = 270;
				if (abs(current_angle - thetha1) < abs(current_angle - thetha2))
				{
					thetha_d = thetha1;
				}
				else
				{
					thetha_d = thetha2;
				}

			}
		}
	}
	req_angle = thetha_d;
	return thetha_d;

}
void Greed::cannon::filter(List<Greed::abs_pos>& ob)
{
	for (int i = 1; i <= 40; i++)//cutoff 40 pixels to show the animation
	{
		ob.del_front();
	}
}

bool Greed::cannon::fireCannon(int s_id)//s_id and s are of target ship
{
	Control con;
	deque<ship*> ship_list = con.getShipList(2369);
	if (ship_list[s_id]->getCurrentHealth() > 0)
	{
		if (ship_list[s_id]->getDiedStatus() == 0 && isShipInMyRadius(s_id, ShipSide::FRONT))
		{

			Greed::bullet bull;
			bull.initialize(true, nullptr, allBullets.howMany(), -1, CANNON_POWER, cannon_id, -1, -1, s_id, true, ::cannon::NA, ShipSide::NA);
			bull.starting_tile = tile;


			bull.ending_tile = ship_list[s_id]->getCurrentTile();
			/*
			else if (isShipInMyRadius(s_id,ShipSide::REAR))
			{
				bull.ending_tile = ship_list[s_id]->getCurrentRearTile();
			}
			*/

			Greed ob;
			Greed::abs_pos launch((double)tile.c * len + (len / 2), (double)tile.r * len + (len / 2));
			Greed::abs_pos target((double)bull.ending_tile.c * len + (len / 2), (double)bull.ending_tile.r * len + (len / 2));

			List<Greed::abs_pos> temp = ob.makeTrajectory(launch.x, launch.y, target.x, target.y);

			filter(temp);//filter to cutoff initial pixels in order to show that the bullet emerges from the cannon hole

			for (int i = 0; i < temp.howMany(); i++)
			{
				bull.bullet_trajectory.push_back(temp[i]);
			}
			//bull.bullet_trajectory.del_rear();
			// bull.bullet_trajectory_cont = bull.bullet_trajectory;
			bullet_list.push_back(bull);
			//allBullets.push_back(bull);

		}
	}
	return true;
}
bool ship::upgradeHealth(int n)// 1 health in 5 money
{
	if (died == 0)
	{
		unique_lock<std::mutex> lk(mutx->m[ship_id]);


		if (n * 5 <= gold)
		{

			unique_lock<std::mutex> lk1(mutx->timeMutex[ship_id]);
			//adding the event in the timeline
			timeline t;
			t.eventype = timeline::EventType::GOLD_TO_HEALTH;
			t.timestamp = graphics::getTotalTime();
			t.h.old = health;

			health += n;
			gold -= 5 * n;
			t.h.n = health;
			this->time_line.push_back(t);
			return true;
		}
	}
	return false;

}
bool ship::upgradeFuel(int n)// 1 fuel in 5 money
{
	if (died == 0)
	{
		unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		if (n * 5 <= gold)
		{
			unique_lock<std::mutex> lk1(mutx->timeMutex[ship_id]);
			//adding the event in the timeline
			timeline t;
			t.eventype = timeline::EventType::GOLD_TO_FUEL;
			t.timestamp = graphics::getTotalTime();
			t.f.old = fuel;
			fuel += n;
			gold -= 5 * n;
			t.f.n = fuel;
			time_line.push_back(t);
			return true;
		}
	}
	return false;
}

bool ship::upgradeAmmo(int n)//1 ammo in 1 money
{
	if (died == 0)
	{
		unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		if (n <= gold)
		{
			unique_lock<std::mutex> lk1(mutx->timeMutex[ship_id]);
			//adding the event in the timeline
			timeline t;
			t.eventype = timeline::EventType::GOLD_TO_AMMO;
			t.timestamp = graphics::getTotalTime();
			t.a.old = ammo;
			ammo += n;
			gold -= n;
			t.a.n = ammo;
			time_line.push_back(t);
			return true;
		}
	}
	return false;
}
void ship::setPath_collision()
{
	int temp1 = path.howMany();
	for (int i = 0; i < temp1; i++)
	{
		path.del_front();
	}
	int temp2 = tile_path.howMany();
	for (int i = 0; i < temp2; i++)
	{
		tile_path.del_front();
	}
	pointPath = 0;
	pointTilePath = 0;

}
void ship::update_pos_collision()
{
	//updating tile_pos_front here
	unique_lock<mutex> lk(mutx->m[this->ship_id]);//locking the mutex
	Greed::abs_pos current;
	current.x = this->tile_pos_rear.c * len;
	double x = current.x;

	current.y = this->tile_pos_rear.r * len;
	double y = current.y;
	Direction d = this->dir;
	Greed::coords final;
	Greed::coords final1l;
	this->absolutePosition = current;


	if (d == Direction::NORTH || d == Direction::WEST || d == Direction::NORTH_WEST)//check for top left corner of the sprite i.e x and y
	{


		front_abs_pos.x = current.x;
		front_abs_pos.y = current.y;

	}
	else if (d == Direction::EAST || d == Direction::NORTH_EAST)//check with top right corner of the sprite.
	{



		front_abs_pos.x = (current.x + len);
		front_abs_pos.y = current.y;

	}
	else if (d == Direction::SOUTH || d == Direction::SOUTH_WEST)//using the bottom left corner
	{

		front_abs_pos.x = current.x;
		front_abs_pos.y = current.y + len;
	}
	else if (d == Direction::SOUTH_EAST)
	{

		front_abs_pos.x = (current.x + len);
		front_abs_pos.y = (current.y + len);
	}
	//updating tile_pos_rear

	if (d == Direction::SOUTH || d == Direction::SOUTH_EAST || d == Direction::EAST)//cheking for top left corner
	{


		rear_abs_pos.x = current.x;
		rear_abs_pos.y = current.y;

	}
	else if (d == Direction::NORTH || d == Direction::NORTH_EAST)//checking for the bottom left corner
	{

		rear_abs_pos.x = x;
		rear_abs_pos.y = y + len;
	}
	else if (d == Direction::SOUTH_WEST || d == Direction::WEST)//top right corner
	{

		rear_abs_pos.x = x + len;
		rear_abs_pos.y = y;
	}
	else if (d == Direction::NORTH_WEST)//checking for the down right corner
	{

		rear_abs_pos.x = x + len;
		rear_abs_pos.y = y + len;
	}
	this->tile_pos_front = this->tile_pos_rear;
	this->rect.setPosition(cx(current.x + origin_x), cy(current.y + origin_y));
}
bool ship::anchorShip_collision()
{
	mutx->mchase[ship_id].lock();

	autopilot = 0;
	mutx->mchase[ship_id].unlock();
	List<Greed::coords> ob;
	setPath_collision();
	update_pos_collision();

	mutx->timeMutex[ship_id].lock();
	timeline t;
	t.timestamp = graphics::getTotalTime();
	t.eventype = timeline::EventType::ANCHOR_SHIP;
	time_line.push_back(t);
	mutx->timeMutex[ship_id].unlock();
	return true;
}
bool ship::anchorShip()
{

	mutx->mchase[ship_id].lock();

	autopilot = 0;
	mutx->mchase[ship_id].unlock();
	List<Greed::coords> ob;
	setPath(ob, 0);


	mutx->timeMutex[ship_id].lock();
	timeline t;
	t.timestamp = graphics::getTotalTime();
	t.eventype = timeline::EventType::ANCHOR_SHIP;
	time_line.push_back(t);
	mutx->timeMutex[ship_id].unlock();
	return true;
}
//for now user2 follows user1

bool get(ship* a, ship* b)
{
	if (a->minutes > b->minutes)
	{
		return true;
	}
	else if (a->minutes == b->minutes && a->seconds != b->seconds)
	{
		return a->seconds > b->seconds;
	}
	else if (a->minutes == b->minutes && a->seconds == b->seconds)
	{
		return a->score > b->score;
	}

}
void graphics::GuiRenderer::list1_renderer()
{
	list1 = tgui::ListView::create();
	list1->setExpandLastColumn(true);
	list1->setSize(::cx(900), ::cy(700));
	list1->setTextSize((::cx(18) + ::cy(18)) / 2);
	list1->setAutoScroll(true);
	list1->setPosition(::cx(0), ::cy(0));
	list1->getRenderer()->setBackgroundColor(sf::Color::Black);
	list1->getRenderer()->setOpacity(0.6);
	list1->getRenderer()->setTextColor(sf::Color::White);
	list1->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
	list1->getRenderer()->setScrollbarWidth((::cx(10) + ::cy(10)) / 2);
	tgui::Scrollbar sc;
	sc.getRenderer()->setTrackColor(sf::Color::White);
	sc.getRenderer()->setOpacity(0.5);
	list1->getRenderer()->setScrollbar(sc.getRenderer()->getData());
	list1->setHeaderTextSize((::cx(20) + ::cy(20)) / 2);
	list1->addColumn("RANK", (::cx(100) + ::cy(100)) / 2, tgui::ListView::ColumnAlignment::Center);
	list1->addColumn("PLAYER_NAME", (::cx(240) + ::cy(240)) / 2, tgui::ListView::ColumnAlignment::Center);
	list1->addColumn("SCORE", (::cx(100) + ::cy(100)) / 2, tgui::ListView::ColumnAlignment::Center);
	list1->addColumn("SHIPS DEFEATED", (::cx(240) + ::cy(240)) / 2, tgui::ListView::ColumnAlignment::Center);
	list1->addColumn("SURVIVAL TIME", (::cx(150) + ::cy(150)) / 2, tgui::ListView::ColumnAlignment::Center);
	final_window->add(list1);

}
void graphics::GuiRenderer::final_window_renderer(tgui::Gui& gui)
{
	final_window = tgui::ChildWindow::create();
	final_window->setSize(::cx(900), ::cy(700));
	final_window->setPosition(::cx(550), ::cy(341));
	final_window->setTitleTextSize((::cx(16) + ::cy(16)) / 2);
	final_window->getRenderer()->setBorderBelowTitleBar(2);
	final_window->getRenderer()->setTitleBarColor(sf::Color(71, 71, 107));
	final_window->getRenderer()->setBackgroundColor(sf::Color::Black);
	final_window->getRenderer()->setOpacity(0.9);
	final_window->setTitle("Summary");
	list1_renderer();
	final_window->add(list1);
	final_window->setVisible(false);
	gui.add(final_window);
}
void graphics::GuiRenderer::health_of_cannon_renderer(tgui::Gui& gui, tgui::Font& font)
{
	hoc = tgui::Label::create();
	hoc->setAutoSize(true);
	hoc->getRenderer()->setFont(font);
	hoc->getRenderer()->setBackgroundColor(sf::Color::Black);
	hoc->getRenderer()->setOpacity(0.8);
	hoc->getRenderer()->setTextColor(sf::Color::White);
	hoc->setTextSize((::cx(22) + ::cy(22)) / 2);
	hoc->setVisible(false);
	gui.add(hoc);
}

void graphics::GuiRenderer::file_renderer(tgui::Gui& gui)
{
	file = tgui::FileDialog::create();
	file->setSize(::cx(600), ::cy(600));
	file->setPosition(::cx(500), ::cy(500));
	file->getRenderer()->setBackgroundColor(sf::Color::Black);
	file->getRenderer()->setOpacity(0.8);
	//file->setPath("F:\\");
	gui.add(file);

}
void graphics::GuiRenderer::tb1_callback(bool check)
{
	if (check == true)
	{
		showRadius = true;
	}
	else
	{
		showRadius = false;

	}
}
void graphics::GuiRenderer::tb2_callback(bool check)
{
	if (check == true)
	{
		showPatterns = true;
	}
	else
	{
		showPatterns = false;
	}
}
void graphics::GuiRenderer::tb2_renderer()
{
	//tgui::Theme th("Black.txt");

	tb2 = tgui::CheckBox::create();
	//tb1->setRenderer(th.getRenderer("ToggleButton"));
	tb2->setPosition(::cx(10), ::cy(120));
	tb2->setSize(::cx(20), ::cy(20));
	tb2->setText("Show tile patterns");
	tb2->setTextClickable(true);
	tb2->setTextSize((::cx(16) + ::cy(16)) / 2);
	tb2->getRenderer()->setTextColor(sf::Color::White);
	tb2->getRenderer()->setTextColorChecked(sf::Color(0, 102, 0));
	tb2->getRenderer()->setTextColorHover(sf::Color::Blue);
	tb2->onChange(&graphics::GuiRenderer::tb2_callback, this);
	tb2->setChecked(showPatterns);

	sp2->add(tb2);
}
void graphics::GuiRenderer::tb1_renderer()
{
	//tgui::Theme th("Black.txt");

	tb1 = tgui::CheckBox::create();
	//tb1->setRenderer(th.getRenderer("ToggleButton"));
	tb1->setPosition(::cx(10), ::cy(90));
	tb1->setSize(::cx(20), ::cy(20));
	tb1->setText("Show Ship Radius");
	tb1->setTextClickable(true);
	tb1->setTextSize((::cx(16) + ::cy(16)) / 2);
	tb1->getRenderer()->setTextColor(sf::Color::White);
	tb1->getRenderer()->setTextColorChecked(sf::Color(0, 102, 0));
	tb1->getRenderer()->setTextColorHover(sf::Color::Blue);
	tb1->onChange(&graphics::GuiRenderer::tb1_callback, this);
	tb1->setChecked(showRadius);
	//tb1->onClick(&graphics::GuiRenderer::tb1_callback, this);
	sp2->add(tb1);
}
void graphics::GuiRenderer::line2_renderer()
{
	line2 = tgui::SeparatorLine::create();
	line2->setSize(::cx(500), ::cy(1));
	line2->setPosition(::cx(0), ::cy(60));
	line2->getRenderer()->setColor(sf::Color::White);
	sp2->add(line1);
}
void graphics::GuiRenderer::line1_renderer()
{
	line1 = tgui::SeparatorLine::create();
	line1->setSize(::cx(500), ::cy(1));
	line1->setPosition(::cx(0), ::cy(60));
	line1->getRenderer()->setColor(sf::Color::White);
	sp2->add(line1);
}
void graphics::GuiRenderer::sliderValueChange(int val)
{
	frame_rate = val;
	string s = "Frame Rate: " + to_string(val);
	frameRateSliderText->setText(s);
	window->setFramerateLimit(val);
}
void graphics::GuiRenderer::frameRateSlider_renderer()
{
	frameRateSlider = tgui::Slider::create();
	frameRateSlider->setSize(::cx(300), ::cy(10));
	frameRateSlider->setMinimum(10);
	frameRateSlider->setMaximum(240);
	frameRateSlider->setValue(frame_rate);

	frameRateSlider->setChangeValueOnScroll(1);
	frameRateSlider->setStep(1);
	frameRateSlider->setVerticalScroll(false);
	frameRateSlider->setPosition(::cx(99), ::cy(40));
	frameRateSlider->onValueChange(&graphics::GuiRenderer::sliderValueChange, this);
	sp2->add(frameRateSlider);
}
void graphics::GuiRenderer::setting_button_callback(tgui::Gui& gui)
{
	childWindowRenderer(gui);
}
void graphics::GuiRenderer::frame_rate_slider_renderer()
{
	frameRateSliderText = tgui::Label::create();
	frameRateSliderText->setAutoSize(true);
	frameRateSliderText->setPosition(::cx(0), ::cy(0));
	string s = "Frame Rate: " + to_string((int)frame_rate);
	frameRateSliderText->setText(s);
	frameRateSliderText->getRenderer()->setTextColor(sf::Color::White);
	//frameRateSliderText->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
	frameRateSliderText->setTextSize((::cx(18) + ::cy(18)) / 2);
	frameRateSliderText->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	sp2->add(frameRateSliderText);

}
void graphics::GuiRenderer::sp2_renderer()
{
	sp2 = tgui::ScrollablePanel::create();
	sp2->setSize(::cx(500), ::cy(500));
	sp2->setPosition(::cx(0), ::cy(0));
	sp2->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	sp2->getRenderer()->setScrollbarWidth(5);
	sp2->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
	frameRateSlider_renderer();
	frame_rate_slider_renderer();
	line1_renderer();
	tb1_renderer();
	tb2_renderer();


}
void graphics::GuiRenderer::childWindowRenderer(tgui::Gui& gui)
{
	settings_window = tgui::ChildWindow::create();
	settings_window->setSize(::cx(500), ::cy(500));
	settings_window->setPosition(::cx(676), ::cy(341));
	settings_window->setTitleTextSize((::cx(16) + ::cy(16)) / 2);
	settings_window->getRenderer()->setBorderBelowTitleBar(2);
	//settings_window->getRenderer()->setBackgroundColor(sf::Color(209, 209, 224));
	settings_window->getRenderer()->setTitleBarColor(sf::Color(71, 71, 107));

	settings_window->setTitle("Settings");
	sp2_renderer();
	settings_window->add(sp2);
	gui.add(settings_window);

}
void graphics::GuiRenderer::gameOverRenderer(sf::Font& font)
{
	gameOver.setString("GAME OVER");
	gameOver.setCharacterSize((::cx(300) + ::cy(300)) / 2);
	gameOver.setColor(sf::Color::White);
	gameOver.setFillColor(sf::Color(255, 255, 255, 180));
	gameOver.setPosition(::cx(100), ::cy(350));//position of  the text
	gameOver.setFont(font);
}
void graphics::GuiRenderer::winner_renderer(sf::Font& font)
{
	winner_text.setString("WINNER IS :");
	winner_text.setFont(font);
	winner_text.setColor(sf::Color::White);
	winner_text.setFillColor(sf::Color(255, 255, 255, 180));
	winner_text.setPosition(::cx(250), ::cy(650));
	winner_text.setCharacterSize((::cx(100) + ::cy(100)) / 2);
}
void graphics::GuiRenderer::time_line_text_renderer(tgui::Gui& gui, tgui::Font& font)
{
	time_line_text = tgui::Label::create();
	time_line_text->getRenderer()->setFont(font);
	time_line_text->setAutoSize(true);
	time_line_text->setPosition(::cx(1619 - 1500), ::cy(601 - 200));
	time_line_text->getRenderer()->setTextColor(sf::Color::White);
	//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
	time_line_text->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
	time_line_text->setTextSize((::cx(38) + ::cy(38)) / 2);
	time_line_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	//name_id->setText("hola");
	time_line_text->setText("TIMELINE");
	//gui.add(time_line_text);
	stats_view_panel->add(time_line_text);
}
void graphics::GuiRenderer::timer_renderer(tgui::Gui& gui)
{
	timer = tgui::Label::create();
	timer->setAutoSize(true);
	timer->setPosition(::cx(845), ::cy(103));
	timer->getRenderer()->setBackgroundColor(sf::Color::Black);
	timer->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
	timer->setTextSize((::cx(42) + ::cy(42)) / 2);
	timer->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	timer->setText("00:00");
	scrollable_panel->add(timer);
}
void graphics::GuiRenderer::closeButton_callback()
{
	stats_view_panel->setVisible(false);
}
void graphics::GuiRenderer::closeButton_renderer(sf::Texture& tex)
{
	closeButton = tgui::Button::create();
	closeButton->setSize(::cx(tex.getSize().x), ::cy(tex.getSize().y));
	closeButton->getRenderer()->setTexture(tex);
	closeButton->setPosition(::cx(400), ::cy(5));
	closeButton->getRenderer()->setTextureHover(tex);
	closeButton->getRenderer()->setTextureFocused(tex);
	closeButton->getRenderer()->setBorderColor(sf::Color::Black);
	closeButton->getRenderer()->setBorderColorHover(sf::Color::Black);
	closeButton->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
	closeButton->getRenderer()->setBorderColorFocused(sf::Color::Black);
	closeButton->onPress(&graphics::GuiRenderer::closeButton_callback, this);
}
void graphics::GuiRenderer::time_line_renderer(tgui::Gui& gui)
{
	for (int i = 0; i < no_of_players; i++)
	{
		auto timeline = tgui::ListView::create();
		timeline->setExpandLastColumn(true);
		timeline->setSize(::cx(445), ::cy(430));
		timeline->setTextSize((::cx(18) + ::cy(18)) / 2);
		timeline->setAutoScroll(true);
		timeline->setPosition(::cx(0), ::cy(449));
		timeline->getRenderer()->setBackgroundColor(sf::Color::Black);
		timeline->getRenderer()->setOpacity(0.6);
		timeline->getRenderer()->setTextColor(sf::Color::White);
		timeline->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
		timeline->getRenderer()->setScrollbarWidth(10);
		tgui::Scrollbar sc;
		sc.getRenderer()->setTrackColor(sf::Color::White);
		sc.getRenderer()->setOpacity(0.5);
		timeline->getRenderer()->setScrollbar(sc.getRenderer()->getData());
		timeline->setHeaderTextSize((::cx(20) + ::cy(20)) / 2);
		timeline->addColumn("Timestamp", (::cx(150) + ::cy(150)) / 2, tgui::ListView::ColumnAlignment::Center);
		timeline->addColumn("Event", (::cx(240) + ::cy(240)) / 2, tgui::ListView::ColumnAlignment::Center);
		this->timeline.push_back(timeline);
		stats_view_panel->add(this->timeline[this->timeline.size() - 1]);
	}

}
void graphics::GuiRenderer::stats_view_renderer()
{

	stats_view = tgui::ListView::create();
	stats_view->setExpandLastColumn(true);
	stats_view->setSeparatorWidth(2);
	stats_view->setSize(::cx(450), ::cy(350));
	stats_view->setTextSize((::cx(18) + ::cy(18)) / 2);
	stats_view->getRenderer()->setBackgroundColor(sf::Color::Black);
	stats_view->getRenderer()->setOpacity(0.6);
	stats_view->getRenderer()->setTextColor(sf::Color::White);
	stats_view->setHeaderTextSize((::cx(20) + ::cy(20)) / 2);
	//adding the columns here
	stats_view->addColumn("S.No.", (::cx(60) + ::cy(60)) / 2);
	stats_view->addColumn("ATTRIBUTE", (::cx(200) + ::cy(200)) / 2, tgui::ListView::ColumnAlignment::Center);
	stats_view->addColumn("VALUE", (::cx(150) + ::cy(150)) / 2, tgui::ListView::ColumnAlignment::Center);
	//adding the values
	vector<vector<tgui::String>> items = { {"1.","Health"},{"2.","Gold"},{"3.","Fuel"},{"4.","Ammo"},{"5.","Tile coords(front)"},{"6.","Tile coords(rear)"},{"7.","Abs. Position Front"},{"8.","Abs. Position Rear"},{"9.","Direction"},{"10.","Radius"},{"11.","Cannon Radius"},{"12.","Cannon Power"},{"13.","died?"},{"14.","Shield"} };
	stats_view->addMultipleItems(items);
	//stats_view->setItemIcon(0, tex);


	//listView->setHeaderHeight(250);
	//setPosition of listView
	stats_view->setPosition(::cx(0), ::cy(50));
	//gui.add(stats_view,"stats_view");

}
void graphics::GuiRenderer::stats_view_panel_renderer(tgui::Group::Ptr g)
{
	stats_view_panel = tgui::ScrollablePanel::create();
	stats_view_panel->setSize(::cx(450), ::cy(900));
	stats_view_panel->setPosition(::cx(1490), ::cy(200));
	stats_view_panel->getRenderer()->setOpacity(0.8);
	stats_view_panel->getRenderer()->setBackgroundColor(sf::Color::Black);

	stats_view_panel->add(closeButton);
	stats_view_panel->add(stats_view);
	stats_view_panel->setVisible(false);
	stats_view_panel->getRenderer()->setScrollbarWidth(10);
	stats_view_panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
	g->add(stats_view_panel);
}
void graphics::GuiRenderer::doIt()
{

	//string health_str = to_string(pl1[gui_renderer.s_id]->getCurrentHealth());
	//const tgui::String h = static_cast<tgui::String>(health_str);
	stats_view->setPosition(::cx(100), ::cy(100));
}
void graphics::GuiRenderer::menu_button_callback(int i, tgui::Gui& gui)
{
	m.lock();
	stats_view_panel->setVisible(true);
	for (int i = 0; i < no_of_players; i++)
	{
		timeline[i]->setVisible(false);
	}
	s_id = i;
	timeline[s_id]->setVisible(true);
	m.unlock();
}
void graphics::GuiRenderer::menu_button_renderer(sf::Texture& tex, tgui::Gui& gui)
{
	for (int i = 0; i < no_of_players; i++)
	{

		auto p = tgui::Button::create();
		p->setSize(::cx(tex.getSize().x), ::cy(tex.getSize().y));
		//call setTexture function 
		p->getRenderer()->setTexture(tex);
		p->setPosition(::cx(10), ::cy(10));
		p->getRenderer()->setTextureHover(tex);
		//p->getRenderer()->setTextureDown(tex);
		//p->getRenderer()->setTextureDownHover(tex);
		p->getRenderer()->setTextureFocused(tex);//this is the thing i wanted
		p->getRenderer()->setBorderColor(sf::Color::Black);
		p->getRenderer()->setBorderColorHover(sf::Color::Black);
		p->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
		p->getRenderer()->setBorderColorFocused(sf::Color::Black);
		p->onPress(&graphics::GuiRenderer::menu_button_callback, this, i, ref(gui));
		player_panels[i]->add(p);
		menu_button.push_back(p);
	}
}
void graphics::GuiRenderer::setting_button_renderer(sf::Texture& tex1, sf::Texture& tex2, tgui::Gui& gui)
{
	settings = tgui::Button::create();
	settings->setPosition(::cx(729), ::cy(3));
	settings->setSize(::cx(tex1.getSize().x), ::cy(tex1.getSize().y));
	settings->getRenderer()->setTexture(tex1);
	settings->getRenderer()->setTextureHover(tex2);
	settings->getRenderer()->setTextureFocused(tex1);
	settings->getRenderer()->setTextureDown(tex2);
	settings->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
	settings->onPress(&graphics::GuiRenderer::setting_button_callback, this, ref(gui));
	scrollable_panel->add(settings);

}
void graphics::GuiRenderer::fuel_renderer1(sf::Texture& tex)
{
	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Picture::create();
		p->getRenderer()->setTexture(tex);
		p->setPosition(::cx(128), ::cy(118));
		player_panels[i]->add(p);
		fuel_icon.push_back(p);
	}
}
void graphics::GuiRenderer::gold_renderer1(sf::Texture& tex)
{
	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Picture::create();
		p->getRenderer()->setTexture(tex);
		p->setPosition(::cx(121), ::cy(75));
		player_panels[i]->add(p);
		gold_icon.push_back(p);
	}
}
void graphics::GuiRenderer::firing_renderer(sf::Texture& tex)
{
	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Picture::create();
		p->getRenderer()->setTexture(tex);
		p->setPosition(::cx(202), ::cy(6));
		player_panels[i]->add(p);
		p->setVisible(false);
		firing_icon.push_back(p);


	}
}
void graphics::GuiRenderer::fire_renderer1(sf::Texture& tex)
{
	for (int i = 0; i < no_of_players; i++)
	{

		auto p = tgui::Picture::create();
		p->getRenderer()->setTexture(tex);
		p->setPosition(::cx(11), ::cy(121));
		player_panels[i]->add(p);
		fire_icon.push_back(p);
	}
}
void graphics::GuiRenderer::sp_renderer(tgui::Group::Ptr group)//renderer for scrollable panel
{
	scrollable_panel = tgui::ScrollablePanel::create();
	scrollable_panel->setSize(::cx(1920), ::cy(181));
	scrollable_panel->setPosition(::cx(origin_offset_screen_x), ::cy(origin_offset_screen_y));
	//scrollable_panel->getRenderer()->setOpacity(0.5);
	scrollable_panel->getRenderer()->setRoundedBorderRadius(100);
	tgui::Scrollbar sc;
	sc.getRenderer()->setOpacity(0.5);
	scrollable_panel->getRenderer()->setScrollbar(sc.getRenderer()->getData());
	scrollable_panel->getRenderer()->setScrollbarWidth(13);
	scrollable_panel->getRenderer()->setBackgroundColor(sf::Color::Black);
	scrollable_panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
	//scrollable_panel->getRenderer()->setBorders(tgui::Borders::Outline(0,2,0,2));
	//scrollable_panel->getRenderer()->setBorderColor(sf::Color::Blue);
	group->add(scrollable_panel);
}
void graphics::GuiRenderer::p_renderer()
{
	for (int i = 1; i <= no_of_players; i++)
	{
		auto p = tgui::Panel::create();
		player_panels.push_back(p);
	}
	//rendering every panel
	int x = 0;//position of the main scrollable panel
	int y = 0;
	for (int i = 0; i < no_of_players; i++)
	{
		if (x >= 723 && x < 964)
		{
			x = 1100;
		}
		player_panels[i]->setPosition(::cx(x), ::cy(y));
		player_panels[i]->setSize(::cx(240), ::cy(168));
		player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Black);
		player_panels[i]->getRenderer()->setBorders(tgui::Borders::Outline(1));
		player_panels[i]->getRenderer()->setBorderColor(sf::Color::White);
		x += 241;
		scrollable_panel->add(player_panels[i]);

	}
}
void graphics::GuiRenderer::name_id_renderer(tgui::Gui& gui, tgui::Font& font)
{
	//load Font file

	name_id = tgui::Label::create();
	name_id->getRenderer()->setFont(font);
	name_id->setAutoSize(true);
	name_id->setPosition(::cx(175), ::cy(5));
	name_id->getRenderer()->setTextColor(sf::Color::White);
	//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
	name_id->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
	name_id->setTextSize((::cx(24) + ::cy(24)) / 2);
	name_id->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	//name_id->setText("hola");
	stats_view_panel->add(name_id);

}
void graphics::GuiRenderer::player_name_renderer(tgui::Font& font)
{
	int x = player_name_coord.x;
	int y = player_name_coord.y;
	for (int i = 0; i < no_of_players; i++)
	{

		auto p = tgui::Label::create();
		p->setSize(::cx(200), ::cy(35));
		p->setPosition(::cx(x), ::cy(y));//18,3 is the offset of the player name label
		p->getRenderer()->setFont(font);
		//the text is set in callable function
		p->getRenderer()->setTextColor(sf::Color::White);
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(24) + ::cy(24)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		player_name.push_back(p);
	}
}
void graphics::GuiRenderer::score_renderer1()//for printing the simple text "score:"
{
	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Label::create();
		//p->setSize(::cx(80), ::cy(35));
		p->setAutoSize(true);
		p->setPosition(::cx(10), ::cy(45));
		p->setText("Score:");
		p->getRenderer()->setTextColor(sf::Color::White);
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(16) + ::cy(16)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		score.push_back(p);
	}
}
void graphics::GuiRenderer::score_renderer2()//for printing the score in realtime
{
	//iterate the below code over no_of_players times


	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Label::create();
		//p->setSize(::cx(80), ::cy(35));
		p->setAutoSize(true);
		p->setPosition(::cx(72), ::cy(46));
		p->setText("10000");
		p->getRenderer()->setTextColor(sf::Color(255, 102, 0));
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(16) + ::cy(16)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		score_value.push_back(p);
	}
}
void graphics::GuiRenderer::health_renderer(sf::Texture& tex)
{
	for (int i = 0; i < no_of_players; i++)
	{

		auto p = tgui::Picture::create();
		p->getRenderer()->setTexture(tex);
		p->setPosition(::cx(9), ::cy(79));
		player_panels[i]->add(p);
		health_icon.push_back(p);
	}
}
void graphics::GuiRenderer::health_value_renderer()
{
	//iterate it for no_of_players times

	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Label::create();
		p->setAutoSize(true);
		p->setPosition(::cx(49), ::cy(81));
		p->setText("10000");
		p->getRenderer()->setTextColor(sf::Color(255, 0, 0));
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(16) + ::cy(16)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		health_value.push_back(p);
	}
}
void graphics::GuiRenderer::fire_value_renderer()
{
	//iterate it for no_of_players times

	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Label::create();
		p->setAutoSize(true);
		p->setPosition(::cx(49), ::cy(125));
		p->setText("10000");
		p->getRenderer()->setTextColor(sf::Color(51, 51, 255));
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(16) + ::cy(16)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		fire_value.push_back(p);
	}
}
void graphics::GuiRenderer::gold_value_renderer()
{
	//iterate it for no_of_players times

	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Label::create();
		p->setAutoSize(true);
		p->setPosition(::cx(167), ::cy(81));
		p->setText("10000");
		p->getRenderer()->setTextColor(sf::Color(51, 51, 255));
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(16) + ::cy(16)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		gold_value.push_back(p);
	}
}
void graphics::GuiRenderer::fuel_value_renderer()
{
	//iterate it for no_of_players times

	for (int i = 0; i < no_of_players; i++)
	{
		auto p = tgui::Label::create();
		p->setAutoSize(true);
		p->setPosition(::cx(167), ::cy(126));
		p->setText("10000");
		p->getRenderer()->setTextColor(sf::Color(255, 0, 0));
		//p->getRenderer()->setBackgroundColor(sf::Color::Blue);
		p->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		p->setTextSize((::cx(16) + ::cy(16)) / 2);
		p->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		player_panels[i]->add(p);
		fuel_value.push_back(p);
	}
}
void graphics::tileMap::setScene(int w, int h, int xx, int yy, sf::Texture& tex, int code[rows][columns])//here xx and yy are the coordinates
{//this is not complete and final

	//w is number of columns, h is number of rows
	sf::Vertex* temp;
	tile.resize(w * h * 4);
	tile.setPrimitiveType(sf::Quads);
	int x = xx;//origin
	int y = yy;//origin
	int z = 0;



	for (int i = 0; i < h; i++)
	{
		x = xx;
		for (int j = 0; j < w; j++)
		{
			temp = &tile[z];
			//temp[0].position = sf::Vector2f(x, y);
			tile[z].position = sf::Vector2f(::cx(x), ::cy(y));
			tile[z + 1].position = sf::Vector2f(::cx(x + len), ::cy(y));
			temp[2].position = sf::Vector2f(::cx(x + len), ::cy(y + len));
			temp[3].position = sf::Vector2f(::cx(x), ::cy(y + len));

			temp[0].texCoords = sf::Vector2f(tex.getSize().x / 2 * code[i][j], 0);
			temp[1].texCoords = sf::Vector2f(tex.getSize().x / 2 * code[i][j] + tex.getSize().x / 2, 0);
			temp[2].texCoords = sf::Vector2f(tex.getSize().x / 2 * code[i][j] + tex.getSize().x / 2, tex.getSize().y);
			temp[3].texCoords = sf::Vector2f(tex.getSize().x / 2 * code[i][j], tex.getSize().y);
			if (showPatterns)
			{
				temp[2].color = sf::Color::Blue;
			}
			else
			{
				temp[0].color = sf::Color::White;
				temp[2].color = sf::Color::White;
			}
			x += len;
			z += 4;
		}
		y += len;

	}


}

int graphics::animator::frame(double elapsed_time, sf::Sprite& sp)
{

	int frame = (int)(((elapsed_time - starting_time) / animation_time) * no_of_frames) % no_of_frames;
	//cout << "\n frame is==>" << frame;
	sp.setTextureRect(sf::IntRect(::cx(frame * width), 0, ::cx(width), ::cy(width)));
	total_time = elapsed_time - starting_time;
	// cout << "\n ----------------------------------------";
	 /*
	 cout << "\n starting_time==>" << starting_time << " " << "\n elapsed_time==>" << elapsed_time;
	 cout << "\n elapsed_time-starting_time==>" << elapsed_time - starting_time;
	 cout << "\n frame==>" << frame;
	 */
	return frame;
}
bool graphics::checkCollision(int sid, const Greed::bullet& ob)//sid is the ship id of  the victim ship.
{
	Control obb;
	deque<ship*> shipInfoList = obb.getShipList(2369);
	Direction dir = shipInfoList[sid]->getShipDirection();//direction of the target ship
	ship::shipEntity entity = shipInfoList[sid]->getShipEntity();//entity of the target ship
	sf::FloatRect bullet_pos = ob.bullet_entity.getGlobalBounds();
	Greed::abs_pos coord(::rx(bullet_pos.left) - origin_x, ::ry(bullet_pos.top) - origin_y);


	//here ship entity coords are in original form and coord is in pseudo form
	/*
	*original form means without transposition of the coordinates
	* pseudo form means with transposition of the coordinates
	*/
	//converting original form to pseudo form


	int dis = ((::cx(10) + ::cy(10)) / 2);

	if (dir == Direction::EAST || dir == Direction::WEST)
	{

		if (coord.x >= entity.c1.x && coord.x <= entity.c2.x && coord.y >= entity.c1.y + dis && coord.y <= entity.c4.y - dis)
		{
			return true;
		}
		else
			return false;
	}
	else
	{

		if ((coord.y >= entity.c1.y && coord.y <= entity.c4.y) && (coord.x >= entity.c1.x + dis && coord.x <= entity.c2.x - dis))
		{
			return true;
		}
		else
		{
			return false;
		}
	}





}
long double graphics::getTotalTime()
{

	return total_secs;
}
bool graphics::check_game_over(deque<ship*>& pl1)
{
	//killing the ships who have no money,no ammo and no fuel
	for (int i = 0; i < pl1.size(); i++)
	{
		if (pl1[i]->getCurrentFuel() == 0 && pl1[i]->getCurrentAmmo() == 0 && pl1[i]->getCurrentGold() == 0)
		{
			pl1[i]->died = 1;
		}
	}
	int died = 0;
	for (int i = 0; i < pl1.size(); i++)
	{
		if (pl1[i]->getDiedStatus() == 1)
		{
			died++;
		}
	}
	if (died == pl1.size() || died == pl1.size() - 1)//only 1 is left or none are left
	{
		return true;
	}
	return false;

}
deque<ship*> graphics::findWinner(deque<ship*> l)
{
	//finding the winner on the following bases
	 /*
	 * the last alive ship will win
	 * if no ship is alive then the longest surviving ship will win
	 * if two ships have same survival time, then tie will be broken on basis of score
	 */
	sort(l.begin(), l.end(), ::get);
	return l;


}
//insert graphics
void update_frame(deque<ship*>& pl1, pack_ship& ob,int i)
{
	pl1[i]->ship_id = ob.ship_id;	
	pl1[i]->seconds = ob.seconds;
	pl1[i]->minutes = ob.minutes;
	pl1[i]->killer_ship_id = ob.killer_ship_id;
	pl1[i]->killer_cannon_id = ob.killer_cannon_id;
	pl1[i]->killed_ships.clear();
	for (int i = 0; i < ob.killed_ships_size; i++)
	{
		pl1[i]->killed_ships.push_back(ob.killed_ships[i]);
	}
	pl1[i]->score = ob.score;
	pl1[i]->name = ob.name;
	pl1[i]->front_abs_pos = ob.front_abs_pos;
	pl1[i]->rear_abs_pos = ob.rear_abs_pos;
	pl1[i]->dir = ob.dir;
	pl1[i]->radius = ob.radius;
	pl1[i]->bullet_radius = ob.bullet_radius;
	pl1[i]->health = ob.health;
	pl1[i]->gold = ob.gold;
	pl1[i]->died = ob.died;
	pl1[i]->motion = ob.motion;
	pl1[i]->fuel = ob.fuel;
	pl1[i]->invisible = ob.invisible;
	pl1[i]->ammo = ob.ammo;
	pl1[i]->bullet_pointer = ob.bullet_pointer;
	pl1[i]->tile_pos_front = ob.tile_pos_front;
	pl1[i]->tile_pos_front = ob.tile_pos_front;
	pl1[i]->absolutePosition = ob.absolutePosition;
	
	if (ob.isthere == true)
	{
		pl1[i]->time_line.push_back(ob.time_line);
		
	}
	//data ends
}
SOCKET peer_sock = -1;
SOCKET connect_to_server()
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		cout << "\n failed to initialize";
	}
#endif // defined
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	int res = 0;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* server_add;
	char buff[1000];
	getaddrinfo("127.0.0.1", "8080", &hints, &server_add);
	getnameinfo(server_add->ai_addr, server_add->ai_addrlen, buff, sizeof(buff), 0, 0, NI_NUMERICHOST);
	cout << "\n the server address is==>" << buff;
	cout << endl;
	SOCKET peer_socket = socket(server_add->ai_family, server_add->ai_socktype, server_add->ai_protocol);
	cout << "\n socket number of the user is==>" << peer_socket;
	if (!ISVALIDSOCKET(peer_socket))
	{
		cout << "\n socket not created==>" << GETSOCKETERRNO();
	}
	/**/
	if (connect(peer_socket, server_add->ai_addr, server_add->ai_addrlen))
	{
		cout << "\n problem in connecting";
		
	}
	int yes = 1;
	if (setsockopt(peer_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes)) < 0) //disabling nagle's algorithm for speed in sending the data
	{
		fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO());
	}
	//cout << "\n connected with the server..";
	freeaddrinfo(server_add);
	peer_sock = peer_socket;
	res = 1;
	return peer_socket;
}

bool send_file_to_server(string& str, SOCKET peer_socket)//send any text file of any size to the server...
{
	//send this string to the server
	const char* ch2 = str.c_str();
	int text_size = str.length();
	send(peer_socket, (char*)&text_size, sizeof(text_size), 0);//sending the size of the file to the server
	//cout << "\n length is==>" << text_size;
	int bytes = send(peer_socket, ch2, strlen(ch2), 0);//sending the file here
	//cout << "\n data sent is==>\n" << ch2;
	return true;
}
string fileName = "";
class game_gui
{
public:
	tgui::Group::Ptr window1;
	tgui::Button::Ptr button1;//button to connect to the server
	tgui::Group::Ptr window2;//for window2
	tgui::Label::Ptr label1;//for printing connecting....
	tgui::Button::Ptr button2;//for retrying to connect
	tgui::Label::Ptr label2;//for printing could not connect
	tgui::Group::Ptr window3;//for window 3 having name and upload of file
	tgui::EditBox::Ptr editbox;//to enter the name
	tgui::Button::Ptr button3;//to submit the name of the player to the server
	tgui::Group::Ptr window4;
	tgui::FileDialog::Ptr file;
	tgui::Button::Ptr button4;//button to upload the file
	game_gui()
	{
		window1 = tgui::Group::create();
		window1->setVisible(true);
		window2 = tgui::Group::create();
		window2->setVisible(false);
		window3 = tgui::Group::create();
		window3->setVisible(false);
		window4 = tgui::Group::create();
		window4->setVisible(false);
	}
	//make the function to upload the file
	void button4_callback()
	{
		file_renderer();
	}
	void file_callback(vector<tgui::Filesystem::Path> vec)
	{
		if (vec.size() > 0)
		{
			fileName = vec[0].asString().toStdString();
			cout << "\n the filename is=>" << fileName;
		}

	}
	void file_renderer()
	{
		file = tgui::FileDialog::create();
		file->setSize(::cx(600), ::cy(600));
		file->setPosition(::cx(674), ::cy(250));
		file->getRenderer()->setBackgroundColor(sf::Color::Black);
		file->onFileSelect(&game_gui::file_callback, this);
		file->setFileTypeFilters({ {".txt or .cpp",{"*.cpp","*.txt"} } });
		window4->add(file);
	}
	
	void button4_renderer()
	{
		button4 = tgui::Button::create();
		button4->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
		button4->setPosition(::cx(798), ::cy(428));
		button4->setSize(::cx(220), ::cy(100));
		button4->setText("Upload File");
		button4->setTextSize((::cx(32) + ::cy(32)) / 2);
		button4->getRenderer()->setBackgroundColor(sf::Color::Black);
		button4->getRenderer()->setTextColor(sf::Color::White);
		button4->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		button4->getRenderer()->setRoundedBorderRadius(10);
		button4->getRenderer()->setOpacity(0.8);
		button4->onClick(&game_gui::button4_callback, this);
		window4->add(button4);
		
	}
	void button3_callback()
	{
		cout << editbox->getText().toStdString();
		//call the screen to upload the algorithms
	}
	void button3_renderer()
	{
		button3 = tgui::Button::create();
		button3->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
		button3->setPosition(::cx(817), ::cy(427));
		 
		button3->setSize(::cx(200), ::cy(60));
		button3->setText("Next");
		button3->setTextSize((::cx(40) + ::cy(40)) / 2);
		button3->getRenderer()->setBackgroundColor(sf::Color::Black);
		button3->getRenderer()->setTextColor(sf::Color::White);
		button3->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		button3->getRenderer()->setRoundedBorderRadius(10);
		button3->getRenderer()->setOpacity(0.8);
		button3->onClick(&game_gui::button3_callback, this);
		window3->add(button3);
		
	}
	void editbox_renderer()
	{
		editbox = tgui::EditBox::create();
		editbox->setSize(::cx(500), ::cy(80));
		editbox->setPosition(::cx(669), ::cy(269));
		editbox->setDefaultText("Enter player name");
		editbox->setTextSize((::cx(22) + ::cy(22)) / 2);
		editbox->getRenderer()->setBackgroundColor(sf::Color::Black);
		editbox->getRenderer()->setTextColor(sf::Color::White);
		editbox->getRenderer()->setBackgroundColorHover(sf::Color(115, 115, 115));
		editbox->getRenderer()->setBackgroundColorFocused(sf::Color(115, 115, 155));
		editbox->setAlignment(tgui::EditBox::Alignment::Center);

		window3->add(editbox);
	}
	void button1_callback1()
	{
		future<int> res = async(connect_to_server);//thread and promise
		if (res.get() == -1)
		{
			label1->setVisible(false);
			label2->setVisible(true);
			button2->setVisible(true);
		}
		else if (res.get() == 1)
		{
			cout << "\n connected";
		}
	}
	void button1_callback()
	{



		window1->setVisible(false);
		window2->setVisible(true);
		label1->setVisible(true);
		thread t(&game_gui::button1_callback1, this);
		t.detach();

	}
	void button2_callback()
	{
		label1->setVisible(true);
		label2->setVisible(false);
		button2->setVisible(false);
		thread t(&game_gui::button1_callback1, this);
		t.detach();

	}
	void button2_renderer()//to display the button to retry to connect to server
	{
		button2 = tgui::Button::create();
		button2->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
		button2->setPosition(::cx(798), ::cy(428));
		button2->setSize(::cx(200), ::cy(80));
		button2->setText("Retry");
		button2->setTextSize((::cx(40) + ::cy(40)) / 2);
		button2->getRenderer()->setBackgroundColor(sf::Color::Black);
		button2->getRenderer()->setTextColor(sf::Color::White);
		button2->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		button2->getRenderer()->setRoundedBorderRadius(10);
		button2->getRenderer()->setOpacity(0.8);
		button2->setVisible(false);
		button2->onClick(&game_gui::button2_callback, this);
		window2->add(button2);
		
	}
	void label2_renderer()
	{

		label2 = tgui::Label::create();
		label2->setAutoSize(true);

		label2->setText("could not connect, please retry...");
		label2->setTextSize((::cx(40) + ::cy(40)) / 2);
		 
		label2->setPosition(::cx(579), ::cy(340));
		//label1->getRenderer()->setBackgroundColor(sf::Color::Black);
		label2->getRenderer()->setTextColor(sf::Color::White);
		label2->setVisible(false);
		window2->add(label2);
		
	}
	void label1_renderer()
	{
		label1 = tgui::Label::create();
		label1->setAutoSize(true);
		
		label1->setText("connecting...");
		label1->setTextSize((::cx(40) + ::cy(40)) / 2);
		 
		label1->setPosition(::cx(799), ::cy(438));
		//label1->getRenderer()->setBackgroundColor(sf::Color::Black);
		label1->getRenderer()->setTextColor(sf::Color::White);
		window2->add(label1);
	}
	
	void button1_renderer()
	{
		button1 = tgui::Button::create();
		button1->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
		 
		button1->setPosition(::cx(693), ::cy(369));
		button1->setSize(::cx(500), ::cy(150));
		button1->setText("Connect To Server");
		button1->getRenderer()->setTextColor(sf::Color::White);
		button1->getRenderer()->setBackgroundColor(sf::Color::Black);
		button1->getRenderer()->setBackgroundColorHover(sf::Color(71, 71, 107));
		button1->getRenderer()->setBackgroundColorDown(sf::Color(133, 133, 173));

		button1->getRenderer()->setOpacity(0.8);
		button1->getRenderer()->setRoundedBorderRadius(10);
		button1->getRenderer()->setTextSize((::cx(32) + ::cy(32)) / 2);
		button1->onClick(&game_gui::button1_callback, this);
		window1->add(button1);
	}
	
};
void display()
{
	window.setFramerateLimit(60);
	tgui::Gui gui(window);
	game_gui ob;
	gui.add(ob.window1);
	gui.add(ob.window2);
	gui.add(ob.window3);
	gui.add(ob.window4);
	ob.button1_renderer();
	ob.button2_renderer();
	ob.label1_renderer();
	ob.label2_renderer();
	ob.editbox_renderer();
	ob.button3_renderer();
	ob.button4_renderer();
	
	ob.window1->setVisible(false);
	ob.window2->setVisible(false);
	ob.window3->setVisible(false);
	ob.window4->setVisible(true);

	mover_tgui<tgui::Button::Ptr> moveit;
	//moveit.moving(window, gui, ob.button3);
	

	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();


			if (event.type == sf::Event::Resized)
			{
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}
			gui.handleEvent(event);
		}
		window.clear(sf::Color::Green);
		gui.draw();
		
		window.display();
		
	}

}
void graphics::callable(Mutex* mutx, int code[rows][columns], Map& map_ob)//taking the ship object so as to access the list of the player
{
	deque<int> dying_ships;
	int ran = 0;
	this->mutx = mutx;
	long total_time = 0;
	//retriving the ship list from the control class
	Control nm;
	deque<ship*>& pl1 = nm.getShipList(2369);
	List<Greed::cannon>& cannon_list = nm.getCannonList(2369);//retiving the cannon list
	cout << "\n number of cannons are==>" << cannon_list.howMany();
	List<Greed::coords> opaque = nm.getOpaqueCoords();

	//sf::RenderWindow window(sf::VideoMode(1970, 1190), "graphics testing");
	tgui::Gui gui(window);

	window.setFramerateLimit(60);
	////////////////////////////////////////////////////////////////////////////////////////////////
	sf::Texture explosion_tex;
	explosion_tex.loadFromFile("map_1 attributes/explosion_set.png");//loading the animation for the explosion
	resizeTexture(explosion_tex);//resizing the textures

	sf::Sprite explosion_sprite;
	explosion_sprite.setTexture(explosion_tex);
	////////////////////////////////////////////////////////////////////////////////////////////////
	List<graphics::animator> animation_list;
	//sf::Sprite rect[2];
	sf::Texture tex_s;
	tex_s.loadFromFile("map_1 attributes/ship_s.png");
	resizeTexture(tex_s);
	cout << "\n size is==>" << tex_s.getSize().x << " " << tex_s.getSize().y;
	sf::Texture tex_n;//texture of north
	tex_n.loadFromFile("map_1 attributes/ship_n.png");
	resizeTexture(tex_n);
	sf::Texture tex_e;
	tex_e.loadFromFile("map_1 attributes/ship_e.png");
	resizeTexture(tex_e);
	sf::Texture tex_w;
	tex_w.loadFromFile("map_1 attributes/ship_w.png");
	resizeTexture(tex_w);
	sf::Texture tex_ne;
	tex_ne.loadFromFile("map_1 attributes/ship_ne.png");
	resizeTexture(tex_ne);
	sf::Texture tex_nw;
	tex_nw.loadFromFile("map_1 attributes/ship_nw.png");
	resizeTexture(tex_nw);
	sf::Texture tex_se;
	tex_se.loadFromFile("map_1 attributes/ship_se.png");
	resizeTexture(tex_se);
	sf::Texture tex_sw;
	tex_sw.loadFromFile("map_1 attributes/ship_sw.png");
	resizeTexture(tex_sw);
	sf::Texture cannon_tex;
	cannon_tex.loadFromFile("map_1 attributes/c0.png");//defualt cannon texture
	resizeTexture(cannon_tex);
	sf::Texture cannon_tex_pack;
	cannon_tex_pack.loadFromFile("map_1 attributes/cannon_set.png");
	resizeTexture(cannon_tex_pack);

	for (int i = 0; i < cannon_list.howMany(); i++)//graphic initializer for the cannon of the map
	{
		cannon_list[i].graphic_initializer(cannon_tex, sf::Vector2f(::cx(cannon_list[i].tile.c * 80 + 40 + origin_x), ::cy(cannon_list[i].tile.r * 80 + 40 + origin_y)));
	}


	for (int i = 0; i < pl1.size(); i++)//graphic initializer for the ship of the user
	{
		pl1[i]->graphics_initializer(tex_n);
	}


	sf::Texture tex;
	map_ob.intialize_graphics(tex);//to set the textures on the map
	sf::RenderStates s;
	s.texture = &tex;
	tileMap ob;
	ob.setScene(columns, rows, origin_x, origin_y, tex, code);

	//working on player now

	sf::Clock clock;
	double elapsed_time = 0;
	int c = 0;
	/*
	sf::View view;
	view.reset(sf::FloatRect(0, 0, 1920, 1080));
	view.setCenter(400, 400);
	view.rotate(50);
	window.setView(view);
	*/
	double t2 = 0;
	int frames = 0;
	Greed::coords* prev = new Greed::coords[pl1.size()];
	for (int i = 0; i < pl1.size(); i++)//setting the previous position
	{
		prev[i] = pl1[i]->tile_pos_front;
	}
	total_time = 0;
	//handling the gui part from here
	//-----------------GUI----------------------------------
	tgui::Font font_bold("Fonts/bold.ttf");
	tgui::Font font1("Fonts/semiBoldItalic.ttf");
	sf::Font font2;
	font2.loadFromFile("Fonts/text.ttf");
	sf::Texture setting1;
	setting1.loadFromFile("map_1 attributes/settings_blue.png");
	resizeTexture(setting1);
	sf::Texture setting2;
	setting2.loadFromFile("map_1 attributes/settings_red.png");
	resizeTexture(setting2);
	sf::Texture health;
	health.loadFromFile("map_1 attributes/health.png");
	resizeTexture(health);
	sf::Texture fire;
	fire.loadFromFile("map_1 attributes/fire.png");
	resizeTexture(fire);
	sf::Texture gold;
	gold.loadFromFile("map_1 attributes/money1.png");
	resizeTexture(gold);
	sf::Texture fuel;
	fuel.loadFromFile("map_1 attributes/fuel1.png");
	resizeTexture(fuel);
	sf::Texture menu_texture;
	menu_texture.loadFromFile("map_1 attributes/menu.png");
	resizeTexture(menu_texture);
	sf::Texture close_button;
	close_button.loadFromFile("map_1 attributes/close-window.png");
	resizeTexture(close_button);
	GuiRenderer gui_renderer(pl1.size(), &window);
	tgui::Theme::setDefault("TransparentGrey.txt");
	auto group1 = tgui::Group::create();//group for the top stats view 
	auto group2 = tgui::Group::create();//group for the panel view of the stats
	
	sf::CircleShape bullet_entity;//entity for all the bullets
	bullet_entity.setRadius((cx(5) +cy(5)) / 2);
	bullet_entity.setFillColor(sf::Color::Black);

	
	//gui function call starts

	gui_renderer.sp_renderer(group1);
	gui_renderer.p_renderer();
	gui_renderer.player_name_renderer(font1);
	gui_renderer.score_renderer1();
	gui_renderer.fire_renderer1(fire);
	gui_renderer.health_renderer(health);
	gui_renderer.gold_renderer1(gold);
	gui_renderer.fuel_renderer1(fuel);
	gui_renderer.score_renderer2();

	gui_renderer.fire_value_renderer();
	gui_renderer.gold_value_renderer();

	gui_renderer.health_value_renderer();
	gui_renderer.fuel_value_renderer();
	gui_renderer.menu_button_renderer(menu_texture, gui);
	gui_renderer.closeButton_renderer(close_button);
	gui_renderer.stats_view_renderer();
	gui_renderer.stats_view_panel_renderer(group2);
	gui_renderer.name_id_renderer(gui, font1);
	gui_renderer.time_line_text_renderer(gui, font_bold);
	gui_renderer.gameOverRenderer(font2);
	gui_renderer.winner_renderer(font2);
	gui_renderer.timer_renderer(gui);
	gui_renderer.setting_button_renderer(setting1, setting2, gui);

	gui_renderer.health_of_cannon_renderer(gui, font_bold);
	gui_renderer.final_window_renderer(gui);

	//gui_renderer.childWindowRenderer(gui);
	//gui function calls ends
	for (int i = 0; i < pl1.size(); i++)
	{
		gui_renderer.player_name[i]->setText(pl1[i]->name + "-" + std::to_string(pl1[i]->ship_id));

	}
	//call stats_view_renderer
	gui_renderer.time_line_renderer(gui);

	gui.add(group2);
	gui.add(group1, "first group of widgets");
	//mover_tgui<tgui::RadioButton::Ptr> moveit;

	//moveit.moving(window,gui,gui_renderer.tb1);
	//write a function to check if a number is even or odd
	deque<int> dead_ship;//id of the dead ships
	bool gameOver = false;
	bool display = true;
	vector<int> winner;//id's of the winner of the game, this is a vector in case of a tie
	int var = 0;
	deque<int> deColor;//to decolor the tiles after illuminating them
	deque<ship*> duplicate_pl1;//duplicate for pl1
	
	//starting the networking part from here
	//calling to connect to the server
	cout << "\n connecting to the server";
   SOCKET peer_socket = connect_to_server();
   if (peer_socket > 0)
   {
	   cout << "\n client is connected to the server";
	}

	fd_set master;
	FD_ZERO(&master);
	FD_SET(peer_socket, &master);

	//Greed::abs_pos prev1 = Greed::abs_pos(pl1[0]->rect.getPosition().x,pl1[0]->rect.getPosition().y);
	Greed::abs_pos prev1 = pl1[0]->getAbsolutePosition();
	top_layer ship_data;
	window.setFramerateLimit(60);
	while (window.isOpen())
	{
		fd_set temp_set = master;

		c++;
		frames++;
		sf::Event event;
		sf::Time deltime = clock.restart();
		elapsed_time += deltime.asSeconds();
		total_secs += deltime.asSeconds();
		//total_time += deltime.asSeconds();
		total_time++;
		t2 += deltime.asSeconds();
		if (t2 >= 1 && t2 <= 1.15)
		{
			//cout << "\n frame rate is==>" << c;
			c = 0;
			t2 = 0;
		}
		//
		while (window.pollEvent(event))
		{
			gui.handleEvent(event);
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))

			{
				window.close();

			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
			{
				window.setSize(sf::Vector2u(1920, 1080));
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
			{
				window.setSize(sf::Vector2u(1970, 1190));
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}

			if (event.type == sf::Event::Resized)
			{
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}


		}

		//setting the timer here
		//take the data from the server
		/*
		* data to be taken...
		* 1. location of every ship
		* 2. time left(time will work according to the server
		*
		*/
		int gone = 0;
		int found = 0;
		for (int i = 0; i < cannon_list.howMany(); i++)
		{
			if (cannon_list[i].isDead == false)
			{
				if (cannon_list[i].cannon_sprite.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
				{
					gui_renderer.hoc->setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y - 20);
					gui_renderer.hoc->setText("Health: " + to_string((int)cannon_list[i].health));
					gui_renderer.hoc->setVisible(true);
					found = 1;

				}


			}
		}
		if (found == 0)
		{
			gui_renderer.hoc->setVisible(false);
		}
		for (int i = 0; i < pl1.size(); i++)
		{
			if (pl1[i]->getDiedStatus() == 0)
			{
				if (pl1[i]->rect.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) == true)
				{
					gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color(0, 204, 122));//change the panel color to green
					//now illuminating the radius of the ship chosen
				}
				else
				{
					//gui_renderer.player_panels[i]->getRenderer()->setOpacity(1);
					gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Black);
					int s = deColor.size();

				}
			}
		}

		if (!gameOver)
		{
			//receiving the data

			
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			if (select(peer_socket + 1, &temp_set, NULL, NULL, &timeout) < 0)
			{
				cout << "\n error in select";
			}
			
			int bytes_received = -1;
			if (FD_ISSET(peer_socket, &temp_set))
			{
				memset((void*)&ship_data, 0, sizeof(ship_data));
				//receiving the data
				gone = 1;
				bytes_received = recv(peer_socket, (char*)&ship_data, sizeof(ship_data), 0);
				while (bytes_received < sizeof(ship_data))
				{

					bytes_received += recv(peer_socket, (char*)&ship_data + bytes_received, sizeof(ship_data) - bytes_received, 0);
				}
				if (bytes_received < 1)
				{
					cout << "\n server disconnected the connection";
					CLOSESOCKET(peer_socket);
					continue;
				}
			}



				/*code to update the timer*/
				int min = ship_data.total_secs / 60;
				int sec = (int)ship_data.total_secs % 60;
				gui_renderer.timer->setText(std::to_string(min) + ":" + std::to_string(sec));
				/*code to update the timer ends*/
				for (int i = 0; i < pl1.size(); i++)
				{

					if (gone == 1)
						update_frame(pl1, ship_data.ob[i], i);//to unwrap the data into the ship object


					pl1[i]->rect.setPosition(::cx(pl1[i]->absolutePosition.x + origin_x), ::cy(pl1[i]->absolutePosition.y + origin_y));


					if (pl1[i]->dir == Direction::NORTH)
					{
						pl1[i]->rect.setTexture(tex_n);
					}
					else if (pl1[i]->dir == Direction::SOUTH)
					{
						pl1[i]->rect.setTexture(tex_s);
					}
					else if (pl1[i]->dir == Direction::EAST)
					{
						pl1[i]->rect.setTexture(tex_e);
					}
					else if (pl1[i]->dir == Direction::WEST)
					{
						pl1[i]->rect.setTexture(tex_w);
					}
					else if (pl1[i]->dir == Direction::NORTH_EAST)
					{
						pl1[i]->rect.setTexture(tex_ne);
					}
					else if (pl1[i]->dir == Direction::SOUTH_EAST)
					{
						pl1[i]->rect.setTexture(tex_se);

					}
					else if (pl1[i]->dir == Direction::NORTH_WEST)
					{
						pl1[i]->rect.setTexture(tex_nw);
					}
					else if (pl1[i]->dir == Direction::SOUTH_WEST)
					{
						pl1[i]->rect.setTexture(tex_sw);
					}
					if (pl1[i]->died == 1)
					{
						gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Red);
						gui_renderer.player_panels[i]->getRenderer()->setOpacity(0.6);
					}


				}
				//for rendering and updating the visual stats in the game


				//bullet working for the cannon



				for (int i = 0; i < pl1.size(); i++)
				{
					string s = to_string((int)pl1[i]->getCurrentHealth());
					//updating the gui
					gui_renderer.score_value[i]->setText(to_string(pl1[i]->score));
					gui_renderer.health_value[i]->setText(s);
					gui_renderer.fire_value[i]->setText(to_string((int)pl1[i]->getCurrentAmmo()));
					gui_renderer.gold_value[i]->setText(to_string((int)pl1[i]->getCurrentGold()));
					gui_renderer.fuel_value[i]->setText(to_string((int)pl1[i]->getCurrentFuel()));
					if (pl1[i]->died == 1)
					{
						gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Red);
						gui_renderer.player_panels[i]->getRenderer()->setOpacity(0.6);
					}
				}//for updating the upper boxes
				//to update the timeline listbox

				//writing code to implement the stats feature
				if (gui_renderer.m.try_lock())
				{
					if (gui_renderer.s_id != -1)
					{
						//cout << "\n panel opened for==>" << gui_renderer.s_id;
						string health_str = to_string(pl1[gui_renderer.s_id]->getCurrentHealth());
						const tgui::String h = static_cast<tgui::String>(health_str);
						gui_renderer.name_id->setText(pl1[gui_renderer.s_id]->name + "-" + to_string(pl1[gui_renderer.s_id]->ship_id));

						gui_renderer.stats_view->changeSubItem(0, 2, h);
						//updating gold value
						string gold_str = to_string(pl1[gui_renderer.s_id]->getCurrentGold());
						const tgui::String g = static_cast<tgui::String>(gold_str);
						gui_renderer.stats_view->changeSubItem(1, 2, gold_str);
						//updating Fuel value
						string fuel_str = to_string(pl1[gui_renderer.s_id]->getCurrentFuel());
						const tgui::String f = static_cast<tgui::String>(fuel_str);
						gui_renderer.stats_view->changeSubItem(2, 2, f);
						//updating ammo value
						string ammo_str = to_string(pl1[gui_renderer.s_id]->getCurrentAmmo());
						const tgui::String a = static_cast<tgui::String>(ammo_str);
						gui_renderer.stats_view->changeSubItem(3, 2, a);
						//updating tile coords(front)
						string tcfr = to_string(pl1[gui_renderer.s_id]->getCurrentTile().r);
						string tcfc = to_string(pl1[gui_renderer.s_id]->getCurrentTile().c);
						string fin = "Row: " + tcfr + " " + "Col.: " + tcfc;
						const tgui::String tcf = static_cast<tgui::String>(fin);
						gui_renderer.stats_view->changeSubItem(4, 2, tcf);
						//updating tile coords(back)
						string tcbfr = to_string(pl1[gui_renderer.s_id]->getCurrentRearTile().r);
						string tcbfc = to_string(pl1[gui_renderer.s_id]->getCurrentRearTile().c);
						string fin2 = "Row: " + tcbfr + " " + "Col.: " + tcbfc;
						const tgui::String tcb = static_cast<tgui::String>(fin2);
						gui_renderer.stats_view->changeSubItem(5, 2, tcb);
						//updating abs pos front
						string apfr = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::FRONT).x);
						string apfc = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::FRONT).y);
						string fin3 = "X: " + apfr + " " + "Y: " + apfc;
						const tgui::String apf = static_cast<tgui::String>(fin3);
						gui_renderer.stats_view->changeSubItem(6, 2, apf);
						//updating abs pos back
						string apbfr = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::REAR).x);
						string apbfc = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::REAR).y);
						string fin4 = "X: " + apbfr + " " + "Y: " + apbfc;
						const tgui::String apb = static_cast<tgui::String>(fin4);
						gui_renderer.stats_view->changeSubItem(7, 2, apb);
						//updating direction

						if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::NORTH)
						{
							const tgui::String dir = static_cast<tgui::String>("NORTH");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::EAST)
						{
							const tgui::String dir = static_cast<tgui::String>("EAST");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::WEST)
						{
							const tgui::String dir = static_cast<tgui::String>("WEST");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::SOUTH)
						{
							const tgui::String dir = static_cast<tgui::String>("SOUTH");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::NORTH_EAST)
						{
							const tgui::String dir = static_cast<tgui::String>("NORTH_EAST");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::NORTH_WEST)
						{
							const tgui::String dir = static_cast<tgui::String>("NORTH_WEST");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::SOUTH_EAST)
						{
							const tgui::String dir = static_cast<tgui::String>("SOUTH_EAST");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::SOUTH_WEST)
						{
							const tgui::String dir = static_cast<tgui::String>("SOUTH_WEST");
							gui_renderer.stats_view->changeSubItem(8, 2, dir);
						}
						//updating the radius
						string rad = to_string(pl1[gui_renderer.s_id]->getShipRadius());
						const tgui::String r = static_cast<tgui::String>(rad);
						gui_renderer.stats_view->changeSubItem(9, 2, r);
						//updating cannon radius
						string crad = to_string(pl1[gui_renderer.s_id]->bullet_radius);
						const tgui::String cr = static_cast<tgui::String>(crad);
						gui_renderer.stats_view->changeSubItem(10, 2, cr);
						//updating cannnon power
						string cpow = to_string(pl1[gui_renderer.s_id]->cannon_ob.power);
						const tgui::String cpo = static_cast<tgui::String>(cpow);
						gui_renderer.stats_view->changeSubItem(11, 2, cpo);
						//updating the died status
						if (pl1[gui_renderer.s_id]->getDiedStatus() == 1)
						{
							const tgui::String str = "YES";
							gui_renderer.stats_view->changeSubItem(12, 2, str);
						}
						else if (pl1[gui_renderer.s_id]->getDiedStatus() == 0)
						{
							const tgui::String str = "NO";
							gui_renderer.stats_view->changeSubItem(12, 2, str);
						}
						//updating the shield status
						if (pl1[gui_renderer.s_id]->shield == 1)
						{
							const tgui::String str = "YES";
							gui_renderer.stats_view->changeSubItem(13, 2, str);
						}
						else if (pl1[gui_renderer.s_id]->shield == 0)
						{
							const tgui::String str = "NO";
							gui_renderer.stats_view->changeSubItem(13, 2, str);
						}
						//done updating the stats about the ship
						if (mutx->timeMutex[gui_renderer.s_id].try_lock())//no need to lock the mutex here
						{

							int i = gui_renderer.s_id;
							if (pl1[i]->time_line.size() > 0)
							{
								if (pl1[i]->time_line[0].eventype == timeline::EventType::GOLD_TO_HEALTH)
								{
									vector<tgui::String> arr;
									arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
									string s = "Gold->Health. n=>";
									string s1 = to_string((int)pl1[i]->time_line[0].h.n) + " o=>" + to_string((int)pl1[i]->time_line[0].h.old);
									arr.push_back(static_cast<tgui::String>(s + s1));
									gui_renderer.timeline[i]->addItem(arr);
								}
								else if (pl1[i]->time_line[0].eventype == timeline::EventType::GOLD_TO_FUEL)
								{
									vector<tgui::String> arr;
									arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
									string s = "Gold->Fuel. n=>";
									string s1 = to_string((int)pl1[i]->time_line[0].f.n) + " o=>" + to_string((int)pl1[i]->time_line[0].f.old);
									arr.push_back(static_cast<tgui::String>(s + s1));
									gui_renderer.timeline[i]->addItem(arr);
								}
								else if (pl1[i]->time_line[0].eventype == timeline::EventType::GOLD_TO_AMMO)
								{
									vector<tgui::String> arr;
									arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
									string s = "Gold->Ammo. n=>";
									string s1 = to_string((int)pl1[i]->time_line[0].a.n) + " o=>" + to_string((int)pl1[i]->time_line[0].a.old);
									arr.push_back(static_cast<tgui::String>(s + s1));
									gui_renderer.timeline[i]->addItem(arr);
								}
								else if (pl1[i]->time_line[0].eventype == timeline::EventType::DEFEATED_SHIP)
								{
									vector<tgui::String> arr;
									arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
									string s = "Defeated ship=>" + pl1[pl1[i]->time_line[0].d.id]->name + "(" + to_string(pl1[pl1[i]->time_line[0].d.id]->ship_id) + ")";
									arr.push_back(s);
									gui_renderer.timeline[i]->addItem(arr);
								}
								else if (pl1[i]->time_line[0].eventype == timeline::EventType::ANCHOR_SHIP)
								{
									vector<tgui::String> arr;
									arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
									string s = "Ship is anchored";
									arr.push_back(s);
									gui_renderer.timeline[i]->addItem(arr);
								}
								else if (pl1[i]->time_line[0].eventype == timeline::EventType::SHIP_DIED)
								{
									vector<tgui::String> arr;
									arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
									string s = "Ship is defeated";
									arr.push_back(s);
									gui_renderer.timeline[i]->addItem(arr);
								}
								pl1[i]->time_line.pop_front();

							}
							mutx->timeMutex[i].unlock();
						}

					}
					gui_renderer.m.unlock();
				}//for updating the timeline boxes



				//drawing and animation part starts here
				//cout << "\n testing hello";
				window.clear();

				window.draw(ob.tile, s);
				for (int i = 0; i < pl1.size(); i++)
				{
					if (pl1[i]->died == 0)
					{
						window.draw(pl1[i]->rect);
						//drawing the names of the ships over them 
						name_of_ship.setPosition(::cx(pl1[i]->absolutePosition.x - 5 + origin_x), ::cy(pl1[i]->absolutePosition.y - 10 + origin_y));
						name_of_ship.setString(static_cast<sf::String>(pl1[i]->name));
						window.draw(name_of_ship);
					}

				}
				for (int i = 0; i < pl1.size(); i++)
				{
					if (pl1[i]->died == 0)
						window.draw(pl1[i]->rect);

				}
				//rendering the bullet here
				for (int i = 0; i < ship_data.no_of_bullets; i++)
				{
					bullet_entity.setPosition(cx(ship_data.bullet_pos[i].x + origin_x), cy(ship_data.bullet_pos[i].y + origin_y));
					window.draw(bullet_entity);
				}


				//setting the base sprite on the cannon


				//printing the cannon animation
				for (int i = 0; i < cannon_list.howMany(); i++)
				{
					//setting cannon data here
					cannon_list[i].current_angle = ship_data.cannon_ob[i].angle;
					cannon_list[i].health = ship_data.cannon_ob[i].cannon_health;
					cannon_list[i].isDead = ship_data.cannon_ob[i].is_cannon_dead;

				}
				for (int i = 0; i < cannon_list.howMany(); i++)//setting the default texture of the cannon
				{
					if (cannon_list[i].isDead == true)
					{
						continue;
					}
					cannon_list[i].cannon_sprite.setTexture(cannon_tex);
					cannon_list[i].cannon_sprite.setRotation(cannon_list[i].current_angle);
				}
				for (int i = 0; i < cannon_list.howMany(); i++)
				{
					if (cannon_list[i].isDead == 0)
					{
						//cannon_list[i].cannon_sprite.setRotation(cannon_list[i].current_angle);
						cannon_list[i].cannon_sprite.setTexture(cannon_tex_pack);
						cannon_list[i].cannon_sprite.setTextureRect(sf::IntRect(::cx(ship_data.cannon_ob[i].picture * 80), 0, ::cx(80), ::cy(80)));
						window.draw(cannon_list[i].cannon_sprite);
					}
				}

				for (int i = 0; i < ship_data.no_of_animation; i++)//printing the animations
				{
					explosion_sprite.setPosition(ship_data.animation_ob[i].x, ship_data.animation_ob[i].y);
					explosion_sprite.setTextureRect(sf::IntRect(::cx(ship_data.animation_ob[i].picture * 30), 0, ::cx(30), ::cy(30)));
					window.draw(explosion_sprite);
				}
				ob.setScene(columns, rows, origin_x, origin_y, tex, code);
				if (gui_renderer.showRadius)//to show radius of all the ships
				{

					for (int i = 0; i < pl1.size(); i++)
					{
						if (pl1[i]->getDiedStatus() == 0)
						{
							vector<Greed::coords> l = pl1[i]->getRadiusCoords_ship(i);
							for (int j = 0; j < l.size(); j++)
							{
								int ind = columns * l[j].r * 4 + 4 * l[j].c;
								ob.tile[ind].color = sf::Color::Green;
								ob.tile[ind + 2].color = sf::Color::Green;
								deColor.push_back(ind);

							}
						}
					}
				}

				if (gameOver && ran == 0)
				{

					gui_renderer.final_window->setVisible(true);
					for (int i = 0; i < pl1.size(); i++)
					{
						vector<tgui::String> data;
						data.push_back(static_cast<tgui::String>(i + 1));
						data.push_back(static_cast<tgui::String>(duplicate_pl1[i]->name));
						data.push_back(static_cast<tgui::String>(duplicate_pl1[i]->score));
						data.push_back(static_cast<tgui::String>(duplicate_pl1[i]->killed_ships.size()));
						if (duplicate_pl1[i]->minutes == INT_MAX && duplicate_pl1[i]->seconds == INT_MAX)
						{
							data.push_back("Not Defeated");
						}
						else
							data.push_back(static_cast<tgui::String>(to_string(duplicate_pl1[i]->minutes) + ":" + to_string(duplicate_pl1[i]->seconds)));
						gui_renderer.list1->addItem(data);
					}
					ran = 1;


				}
				if (gameOver)
				{
					window.draw(gui_renderer.gameOver);
				}
				gui.draw();


				window.display();
			
		}

	}
}
class fileData
{
public:
	int no_of_players;
	string player_name;

	fileData()
	{

	}
	fileData(int no, string& name)
	{
		no_of_players = no;
		player_name = name;

	}
};

int main()
{
	//extracting the data


	const int no_of_players = 3;
	
	Control control;
	//creating an object of class Mutex: this object will be passed to every class using mutex
	Mutex mutx;

	//sf::RenderWindow window(sf::VideoMode(1920,1080),"testing window");
	int code[rows][columns];
	List<Greed::coords> opaque_coords;//coordinates of the land 
	Map map1(0);
	map1.initialize_map(code, opaque_coords);
	control.opaque_coords = opaque_coords;
	Greed ob;
	List<Greed::vertex>& l = ship::getGlobalMapref(2369);//no need to change this line
	ob.getMap(code, rows, columns, l); // here globalMap is set
	//initializing globalMap in ship class
	deque<shipInfo> silist;
	List<Greed::coords> l1;
	ship player[11];


	//player[1].updateCost(Map::coords(10,0), 50);

	//player[1].localMap[10 * columns + 0].b.cost = 50;
	deque<ship*> slist;
	
	for (int i = 0; i < no_of_players; i++)
	{
		silist.push_back(shipInfo(&player[i]));
		slist.push_back(&player[i]);
	}
	//generating the coords for spawning the ship
	vector<Greed::coords> spawn;//spawning position for ships having different configurations
	if (no_of_players == 1)//for 0 players
	{
		spawn.push_back(Greed::coords(10, 11));

	}
	else if (no_of_players == 2)
	{

		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));

	}
	else if (no_of_players == 3)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 11));
	}
	else if (no_of_players == 4)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 23));
	}
	else if (no_of_players == 5)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 23));
		spawn.push_back(Greed::coords(10, 11));
	}
	else if (no_of_players == 6)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 23));
		spawn.push_back(Greed::coords(10, 11));
		spawn.push_back(Greed::coords(2, 11));
	}
	else if (no_of_players == 7)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(6, 0));
		spawn.push_back(Greed::coords(6, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 11));
		spawn.push_back(Greed::coords(10, 23));
	}
	for (int i = 0; i < no_of_players; i++)
	{
		player[i].initialize_player("sudoko", &mutx, silist, code, 5, spawn[i]);//last element is the tile pos of the player

	}

	control.setShipList(slist, 2369);
	// player[1].localMap[10 * columns + 0].b.cost = 50;
	//making the cannon lists:
	//let there be 1 cannon only 
	List<Greed::cannon> cannon_list;
	for (int i = 1; i <= 3; i++)
	{
		cannon_list.add_rear(Greed::cannon());

	}

	cannon_list[0].initializer(0, Greed::coords(5, 11));
	cannon_list[1].initializer(1, Greed::coords(3, 6));
	cannon_list[2].initializer(2, Greed::coords(7, 8));

	control.cannon_list = cannon_list;
	ship::cannon_list = cannon_list;
	graphics cg;
	cg.callable(&mutx, code, map1);
	//display();

	char pBuf[256];
	size_t length = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, length);
	cout << "\n file path is==>" << pBuf;

	//code to extract the path of the directory in which the exe is
	string path;
	path.append(pBuf);
	for (int i = path.length() - 1; i >= 0; i--)
	{
		if (path[i] == '\\')
		{
			path.erase(i, path.length());
			break;
		}
	}
	cout << "\n directory is==>" << path;


	/*
	* patch notes 1:
	* to make the game event based.
	* basic mechanism of event based game is as follows:
	* shipsIamInRadiusOf() function
	* iAmInWhoseRadius() function
	*/
}
