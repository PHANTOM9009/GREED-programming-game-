// lib2.cpp : Defines the functions for the static library.
//
#include "pch.h"
#include "framework.h"
#include "greed_offline.hpp"

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
long double Greed::shipCannon::current_bullet = 0;
deque<navigation> resend_navigation;//to store the navigation requests that has be resent
deque<pair<bullet_data, int>> resend_bullet;//firing data that has to be resent

//for moving entites over the screen
template<typename T>

int shall = 0;

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
	shipEntity opp = shipInfoList[s].ob->getShipEntity_without_mutex();//its coordinates will be checked
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



}

int shipInfo::getTotalShips()
{
	return ob->getTotalShips();
}
int shipInfo::getShipId()
{
	return ob->getShipId();
}
Greed::coords shipInfo::getCurrentTile_withoutMutex()
{
	return ob->getCurrenntTile_withoutMutex();
}
Greed::coords shipInfo::getCurrentRearTile_withoutMutex()
{
	return ob->getCurrentRearTile_withoutMutex();
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
		if (c2 < 0)
		{
			c2 = 0;
		}
		else if (c2 > columns)
		{
			c2 = columns - 1;
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

	unique_lock<mutex> lk(mutx->game_tick_mutex_client);


	if (current_count > prev_count)
	{
		prev_count++;
		return true;
	}
	return false;
	/*if (!hector)
	{
		starting_time_limiter = chrono::steady_clock::now();
		hector = true;
	}
	chrono::steady_clock::time_point ending_time_limiter = chrono::steady_clock::now();
	chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(ending_time_limiter - starting_time_limiter);

	elapsed_time += time_span.count();
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
	*/
}

ship::ship()//default ctor for now
{
	this->solid_motion = false;
	this->navigation_promise = false;
	this->anchor_promise = false;
	this->current_count = 0;
	this->prev_count = 0;
	this->collide_count = 0;
	this->hit_bullet_count = 0;
	this->navigation_count = 0;
	this->cost_upgrade_count = 0;
	this->count_upgrade = 0;
	bullet_count = 0;
	client_fire = 0;
	server_fire = 0;
	gameOver = false;
	hector = false;
	lock_ammo = 0;
	lock_fuel = 0;
	lock_health = 0;
	lock_chase_ship = 0;

	frame_rate_limit = 30;
	elapsed_time = 0;
	current_frame_no = -1;
	current_time = chrono::high_resolution_clock::time_point();

	isFiring = false;
	health = 200;
	motion = 0;
	autopilot = 0;

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
	threshold_ammo = 10;
	threshold_fuel = 10;
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
	if (c2 < 0)
	{
		c2 = 0;
	}
	else if (c2 > columns)
	{
		c2 = columns - 1;
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
	if (c2 < 0)
	{
		c2 = 0;
	}
	else if (c2 > columns)
	{
		c2 = columns - 1;
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
			cout << "\n for the ship=>" << ship_id;
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
			deque<Greed::coords> uml;
			for (int i = 0; i < tile_path.howMany(); i++)
			{
				uml.push_back(tile_path[i]);

			}
			deque<Greed::abs_pos> bml;
			for (int i = 0; i < path.howMany(); i++)
			{
				bml.push_back(path[i]);
			}
			cout << "\n----------------------------------------";
			cout << "\n for the ship=>" << ship_id;
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
	//cout << "\n elements in temp is==>";
	for (int i = 0; i < temp.howMany(); i++)
	{
		cout << temp[i].r << " " << temp[i].c << endl;
	}
	ob = temp;

}
void chaseShip1(int s_id, ship& ob)//the famous chasing  a ship function
{
	//how to terminate this thread??
	ob.mutx->mchase[ob.ship_id].lock();
	ob.autopilot = 1;
	ob.mutx->mchase[ob.ship_id].unlock();

	deque<shipInfo> l = ob.getShipList();
	Greed::path_attribute path;
	Greed::coords temp = l[s_id].getCurrentTile();

	path = ob.setTarget(l[s_id].getCurrentTile());
	ob.setPath(path.target);

	while (1)
	{
		unique_lock<mutex> lk(ob.mutx->gameOver_check);
		if (ob.gameOver)
		{
			//cout << "\n from chaseShip, breaking because of ob.gameOver is true";
			break;
		}
		lk.unlock();

		if (ob.getShipList()[s_id].getDiedStatus() == 1)
		{
			//cout << "\n breaking from chaseship because the ship i was chasing died...................";
			unique_lock<mutex> lk(ob.mutx->mchase[ob.ship_id]);
			ob.autopilot = 0;

			break;
		}

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
				//cout << "\n breaking because autopilot is false";
				break;
			}
			if (ob.died == 1)
			{
				ob.mutx->mchase[ob.ship_id].unlock();
				break;
			}
			ob.mutx->mchase[ob.ship_id].unlock();
		}
		Sleep(200);
	}




}
bool ship::setPath(List<Greed::coords> ob, int state)  //for adding the supplied list to  the Linked list of the user movement
{
	/*if state is:
	* 1-> it means a destination is set
	* 0-> no destination is set and the ship wants to anchor
	*/
	//locking the mutex first
	unique_lock<mutex> lk(mutx->m[ship_id]);
	if (path.howMany() > 0)
	{

		//mutx->m[ship_id].lock();
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



		}


		//mutx->m[ship_id].unlock();
		return true;
	}
	else//if the path list is empty
	{
		if (state == 1)
		{
			//mutx->m[ship_id].lock();
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


			//mutx->m[ship_id].unlock();
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
	att.entity = graph[index].b.tile_id;
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
	unique_lock<mutex> lk(mutx->m[ship_id]);
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
		lk.unlock();
		::filter(tile_path, con.opaque_coords, *this);//function to filter out of range coordinates
		/*
		cout << "\n tile path after filtering is==>";
		for (int i = 0; i < tile_path.howMany(); i++)
		{
			cout << tile_path[i].r << " " << tile_path[i].c << endl;
		}
		*/
		if (tile_path.howMany() > 0)
		{
			solid_motion = true;
			motion = true;
			navigation_promise = true;
		}
		else
		{
			solid_motion = false;
			motion = false;
			navigation_promise = false;
		}
		setPath(tile_path);
		return true;
	}
	return false;
}

Greed::path_attribute ship::setTarget(Greed::coords ob)
{
	//this function will return the path_attribute object containing information about the exact path to take
	Greed::path_attribute ob1;//to be returned
	mutx->m[ship_id].lock();
	//mutx->m_global_map.lock();
	map_ob.path(tile_pos_front, ob, localMap, ob1, localMap);

	//mutx->m_global_map.unlock();
	mutx->m[ship_id].unlock();
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
	unique_lock<mutex> lk(this->mutx->m[ship_id]);
	if (new_cost >= 0 && map_ob.isValidCoords(ob))
	{
		// int ind=map_ob.Map::find_index(ob,ship::localMap);//index of the coord given for the cost to be updated
		int ind = ob.r * columns + ob.c;
		ship::localMap[ind].b.cost = new_cost;
		return true;
	}
	return false;
}
void ship::Greed_updateCost(Greed::coords ob, double new_cost)
{
	if (new_cost > 0 && map_ob.isValidCoords(ob))
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		map_cost_data.push_back(map_cost(new_cost, ob));
	}
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
	if (l1[s_id]->died == 1)
	{
		return false;
	}
	int radius = l1[s_id]->getShipRadius();
	Greed::coords position;
	if (opponent_side == ShipSide::FRONT)
	{
		position = l1[s_id]->getCurrenntTile_withoutMutex();
	}
	else if (opponent_side == ShipSide::REAR)
		position = l1[s_id]->getCurrentRearTile_withoutMutex();

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
	if (c2 < 0)
	{
		c2 = 0;
	}
	else if (c2 > columns)
	{
		c2 = columns - 1;
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
	if (s_id <= l.size() - 1 && s_id >= 0 && l[s_id]->died == 0)
	{
		Greed::coords pos;
		//chekcing for front side of opponent first
		if (oppSide == ShipSide::FRONT)
		{
			Greed::coords pos = l[s_id]->getCurrenntTile_withoutMutex();
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
			Greed::coords pos = l[s_id]->getCurrentRearTile_withoutMutex();
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
	if (s_id <= l.size() - 1 && s_id >= 0 && l[s_id]->died == 0)
	{
		Greed::coords pos;
		//chekcing for front side of opponent first
		if (oppSide == ShipSide::FRONT)
		{
			Greed::coords pos = l[s_id]->getCurrenntTile_withoutMutex();
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
			Greed::coords pos = l[s_id]->getCurrentRearTile_withoutMutex();
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
	//cout << "\n i came in=>" << this->ship_id;
	if (autopilot == 0)
	{

		//cout << "\n i ran=>"<<this->ship_id;
		thread t(chaseShip1, s_id, ref(*this));
		t.detach();
		lock_chase_ship = 0;

		

	}
	//Sleep(300);
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
double shipInfo::getCurrentAmmo()
{
	return ob->getCurrentAmmo();
}

bool ship::fireAtCannon(int c_id, cannon can = cannon::FRONT)
{
	if (cannon_ob.activeBullets.size() <= 1 && isCannonInRadius(c_id, (ShipSide)(int)can) && ammo > 0 && cannon_list[c_id].isDead == false)//if cannon is in the provided radius of the firing ship
	{

		Control ob;

		Greed::bullet bull;
		bull.initialize(true, mutx, cannon_ob.current_bullet++, this->ship_id, cannon_ob.power, -1, this->ship_id, c_id, -1, true, can, ShipSide::NA);




		// cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;
		cannon_ob.activeBullets.push_back(bull);//only the active bullets are kept here
		//cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;



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
		bull.ending_tile = shipInfoList[s_id].getCurrentTile_withoutMutex();
	}
	else if (s == ShipSide::REAR)
	{
		bull.ending_tile = shipInfoList[s_id].getCurrentRearTile_withoutMutex();
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
	shipEntity entity = shipInfoList[s_id].ob->getShipEntity_without_mutex();//eror here target ship's entity has to be found
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

	if (shipInfoList[s_id].ob->died == 0 && ammo > 0 && isShipInMyRadius_forFire(s_id, can, s) && cannon_ob.activeBullets.size() <= 1)
		//if (ammo > 0 && (s == ShipSide::MIDDLE && (isShipInMyRadius(s_id, ShipSide((int)can), ShipSide::FRONT) && isShipInMyRadius(s_id, ShipSide((int)can), ShipSide::REAR))) || (s != ShipSide::MIDDLE && isShipInMyRadius(s_id, ShipSide((int)can), s)))
	{


		Greed::bullet bull;

		bull.initialize(true, mutx, cannon_ob.current_bullet++, this->ship_id, cannon_ob.power, -1, this->ship_id, -1, s_id, true, can, s);
		//setting the starting and ending tile for the bullet in the game


		// cannon_ob.allBullets.push_back(bull);

		 // cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;
		cannon_ob.activeBullets.push_back(bull);//only the active bullets are kept here
		//cout << "\n info=>" << cannon_ob.allBullets[0].starting_tile.r << " " << cannon_ob.allBullets[0].starting_tile.c;


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
			position = l1[s_id]->getCurrenntTile_withoutMutex();
		}
		else if (opponent_side == ShipSide::REAR)
			position = l1[s_id]->getCurrentRearTile_withoutMutex();

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
		if (c2 < 0)
		{
			c2 = 0;
		}
		else if (c2 > columns)
		{
			c2 = columns - 1;
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

	int ship_id;
	if (current_ship >= 0 && ship_list[current_ship]->died == 0)
	{
		ship_id = current_ship;

	}
	else
	{
		ship_id = -1;
		current_ship = -1;
	}

	/*
	* this is concept2
	* this law dictates that the ship having more health will be beaten first
	* this rule is implemented below
	*/
	int s_id;
	int maxi = INT_MIN;
	for (int i = 0; i < ship_list.size(); i++)
	{
		if (ship_list[i]->died == 0 && isShipInMyRadius(ship_list[i]->getShipId(), ShipSide::FRONT))//the ship is alive
		{
			//cout << "\nfor cannon ship_id==>" << ship_list[i]->getShipId();
			if (maxi < ship_list[i]->health)
			{
				ship_id = i;
				maxi = ship_list[i]->health;

			}
		}
	}
	//cout << "\n ship id=>" << ship_id;
	current_ship = ship_id;
	return ship_id;

}
double Greed::cannon::get_required_angle()//getVictimSHip has to be called in order set a new ship as target
{
	if (current_ship != -1)
	{
		Control con;
		deque<ship*> ship_list = con.ship_list;
		Greed::coords ship_tile;
		if (current_ship != -1)//ss is corrupted!
		{
			ship_tile = ship_list[this->current_ship]->tile_pos_front;
		}
		/*
		else if (current_ship != -1 && ss == ShipSide::REAR)
		{
			ship_tile = ship_list[this->current_ship]->getCurrentRearTile();
		}
		*/
		Greed::abs_pos sabs((ship_tile.c * len) + (len / 2), (ship_tile.r * len) + (len / 2));
		Greed::abs_pos cabs(tile.c * len + (len / 2), tile.r * len + (len / 2));
		double thetha_r = 0;//in radians
		double thetha_d = 0;
		thetha_d = current_angle;//in degree
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
	return 0;

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
	if (ship_list[s_id]->health > 0)
	{
		if (ship_list[s_id]->died == 0 && isShipInMyRadius(s_id, ShipSide::FRONT))
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
		//unique_lock<std::mutex> lk(mutx->m[ship_id]);


		if (n * 5 <= gold)
		{

			//unique_lock<std::mutex> lk1(mutx->timeMutex[ship_id]);
			//adding the event in the timeline
			timeline t;
			t.eventype = timeline::EventType::GOLD_TO_HEALTH;
			
			t.h.old = health;

			health += n;
			gold -= 5 * n;
			t.h.n = health;
			this->time_line.push_back(t);
			unlock.push_back(1);
			//cout << "\n unlocked the seal for the user=>" << this->ship_id;
			return true;
		}
	}
	return false;

}
bool ship::upgradeFuel(int n)// 1 fuel in 5 money
{
	if (died == 0)
	{
		//unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		if (n * 5 <= gold)
		{
			//unique_lock<std::mutex> lk1(mutx->timeMutex[ship_id]);
			//adding the event in the timeline
			timeline t;
			t.eventype = timeline::EventType::GOLD_TO_FUEL;
			t.timestamp = graphics::getTotalTime();
			t.f.old = fuel;
			fuel += n;
			gold -= 5 * n;
			t.f.n = fuel;
			time_line.push_back(t);
			unlock.push_back(2);
			return true;
		}
	}
	return false;
}

bool ship::upgradeAmmo(int n)//1 ammo in 1 money
{
	if (died == 0)
	{
		//unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		if (n <= gold)
		{
			//unique_lock<std::mutex> lk1(mutx->timeMutex[ship_id]);
			//adding the event in the timeline
			timeline t;
			t.eventype = timeline::EventType::GOLD_TO_AMMO;
			t.timestamp = graphics::getTotalTime();
			t.a.old = ammo;
			ammo += n;
			gold -= n;
			t.a.n = ammo;
			time_line.push_back(t);
			unlock.push_back(0);
			return true;
		}
	}
	return false;
}
void ship::setPath_collision()
{
	unique_lock<mutex> lk(mutx->m[ship_id]);
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
	//unique_lock<mutex> lk(mutx->m[this->ship_id]);//locking the mutex
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

}
bool ship::anchorShip_collision()
{
	mutx->mchase[ship_id].lock();

	autopilot = 0;
	mutx->mchase[ship_id].unlock();
	List<Greed::coords> ob;
	setPath_collision();
	update_pos_collision();

	//mutx->timeMutex[ship_id].lock();
	timeline t;
	t.timestamp = graphics::getTotalTime();
	t.eventype = timeline::EventType::ANCHOR_SHIP;
	time_line.push_back(t);
	//mutx->timeMutex[ship_id].unlock();
	return true;
}
bool ship::anchorShip()
{

	mutx->mchase[ship_id].lock();

	autopilot = 0;
	mutx->mchase[ship_id].unlock();
	List<Greed::coords> ob;
	setPath(ob, 0);


	//mutx->timeMutex[ship_id].lock();
	timeline t;
	t.timestamp = graphics::getTotalTime();
	t.eventype = timeline::EventType::ANCHOR_SHIP;
	time_line.push_back(t);
	//mutx->timeMutex[ship_id].unlock();
	return true;
}
//for now user2 follows user1

bool get(ship* a, ship* b)
{
	return a->score > b->score;

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



//for moving entites over the screen

long game_tick = 1;//this is the game tick of the server not the client
void GreedMain(ship& ob);
string username = "username";
string password = "password";
string game_token;
int my_id;//id of the player in the game
string ip_address;//ip to which the client will connect to..
int mode;
SOCKET lobby_socket;//socket having the connection between lobby server and the client, this will be used to send the heartbeat messages
SOCKET sending_socket;//socket to send data to the server
//peer_socket is for recving data from the server

deque<recv_data> input_data;//input data from the server to the client
deque<send_data> terminal_data;


string tournament_id;


bool SEND(SOCKET sock, char* buff, int length)
{
	/*overload of send function of UDP having the mechanism of resending and ack*/
	fd_set master;
	FD_ZERO(&master);
	FD_SET(sock, &master);
	fd_set read;
	FD_ZERO(&read);
	chrono::steady_clock::time_point now = chrono::steady_clock::now();

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int b = send(sock, buff, length, 0);
	if (b < 1)
	{
		cout << "\n cannot send the bytes.." << GETSOCKETERRNO();
		return false;
	}
	while (1)
	{
		read = master;
		select(sock + 1, &read, 0, 0, &timeout);
		if (FD_ISSET(sock, &read))
		{
			int ack;
			int bytes = recv(sock, (char*)&ack, sizeof(ack), 0);
			if (ack == 1)
			{
				return true;
			}
		}
		auto end = chrono::steady_clock::now();
		chrono::duration<double> iteration_time = chrono::duration_cast<chrono::duration<double>>(end - now);
		if (iteration_time.count() > 1)
		{
			//resend the packet
			int bytes = send(sock, buff, length, 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the bytes again=>" << GETSOCKETERRNO();
				return false;
			}

			now = chrono::steady_clock::now();//changing the time since the last message sent
		}

	}
}
bool RECV(SOCKET sock, char* buff, int length)
{
	//cout << "\n from RECV: just entered into the function..";
	/*overload for UDP recv, to recv the data carefully*/
	while (1)
	{
		int b = recv(sock, buff, length, 0);
		//cout << "\n from RECV: received some bytes;";
		if (b > 1)
		{
			int ack = 1;
			int bytes = send(sock, (char*)&ack, sizeof(ack), 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the bytes. . " << GETSOCKETERRNO();
			}

			return true;
		}
	}

}
bool find(int id, int hit[100], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (id == hit[i])
		{
			return true;
		}
	}
	return false;

}

SOCKET connect_to_server(int port)//first connection to the server
{
	char port_str[10];
	// Convert port to string
	sprintf(port_str, "%d", port);
	char port_str1[10];
	sprintf(port_str1, "%d", (port + 2));

	//printf("Configuring remote address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo* peer_address;
	struct addrinfo* peer_address1;

	if (getaddrinfo(ip_address.c_str(), port_str, &hints, &peer_address))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (getaddrinfo(ip_address.c_str(), port_str1, &hints, &peer_address1))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);//for receiving data from the game server

	sending_socket = socket(peer_address1->ai_family,
		peer_address1->ai_socktype, peer_address1->ai_protocol);//for sending data to the game server

	if (!ISVALIDSOCKET(socket_peer)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (!ISVALIDSOCKET(sending_socket)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (connect(sending_socket, peer_address1->ai_addr, peer_address1->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	//sending the greet_client object to the game server, with my code and credentials.

	return socket_peer;

}
int prev_server_tick = 0;//this is to check the client does not accept any old packets from the server
void data_recver(SOCKET socket_listen, Mutex* m)
{
	int found = 0;
	int bytes;
	
	double et = 0;
	int count = 0;
	while (1)
	{
		
		if (et > 1)
		{
			//cout << "\n the count of packet is==>" << count;
			et = 0;
			count = 0;
		}

		recv_data ob;
		int recv_bytes = 0;
		char comp[sizeof(ob)];
		int checker = 0;
		if (found == 0)
		{
			recv(socket_listen, (char*)&checker, sizeof(checker), 0);
		}
		if (checker == 1 || found == 1)
		{
			while (recv_bytes < sizeof(ob))
			{
				char buffer[1000];
				found = 0;
				bytes = recv(socket_listen, (char*)&buffer, sizeof(buffer), 0);
				if (bytes < 1)
				{
					//cout << "\n cannot recv the bytes==>" << GETSOCKETERRNO();
					continue;
				}
				if (bytes == 4)
				{
					int check;
					memcpy(&check, buffer, sizeof(int));
					if (check == 1)
					{
						cout << "\n problem occured breaking...";
						found = 1;
						break;
					}
				}

				memcpy(comp + recv_bytes, buffer, bytes);
				recv_bytes += bytes;
				//cout << "\n recved bytes from the server=>" << bytes << " " << ob.arr[100];
				auto now = std::chrono::system_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
				auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
				auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

				//cout << "\n recved data from the server at the time==> " <<
				//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;


			}
			memcpy(&ob, comp, sizeof(ob));
			if (ob.st == 100 && ob.end == 101)
			{
				prev_server_tick = ob.packet_id;
				unique_lock<mutex> lk(m->recv_terminal);
				input_data.push_back(ob);
				count++;
				m->cond_input_terminal.notify_all();
			}
			else
			{
				cout << "\n faulty data";
			}
		}
		//Sleep(10);
	}
}
long double graphics::getTotalTime()
{

	return total_secs;
}
void data_send(Mutex* m, deque<ship*>& pl1)
{
	
	double ep = 0;
	int count = 0;
	while (1)
	{
		

		unique_lock<mutex> lk(m->send_terminal);
		m->cond_terminal.wait(lk, [] {return !terminal_data.empty(); });
		//terminal_data queue has data now make its copy and clear the queue
		deque<send_data> data;
		for (int i = 0; i < terminal_data.size(); i++)
		{
			data.push_back(terminal_data[i]);

		}
		terminal_data.clear();//clearing the queue
		lk.unlock();

		for (int i = 0; i < data.size(); i++)
		{
			//send the data to the client
			data[i].st = 100;
			data[i].end = 101;
			/*
			if (data[i].shipdata_forServer.size_navigation > 0)
			{
				auto now = std::chrono::system_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
				auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
				auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());
				cout << "\n from client sending part, sent for the set Path function==>" << hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;

			}
			*/
			int limit = 1;//default number of times the pcket will be sent 
			if (data[i].shipdata_forServer.size_bulletData > 0 || data[i].shipdata_forServer.size_navigation > 0 || data[i].shipdata_forServer.size_update_cost > 0 || data[i].shipdata_forServer.size_upgrade_data > 0)
			{
				limit = 5;
			}
			for (int k = 1; k <= limit; k++)
			{
				send_data ob = data[i];
				//cout << "sending the packet==>" << ob.packet_id;
				char buffer[sizeof(data[i])];
				memcpy(buffer, &ob, sizeof(ob));

				int sent_bytes = 0;
				//sending that starting a new packet
				int sending_new = my_id;//sending my id to the person

				send(sending_socket, (char*)&sending_new, sizeof(sending_new), 0);
				while (sent_bytes < sizeof(ob))
				{
					int fuck = sizeof(ob) - sent_bytes;
					int bytesToSend = min(MAX_LENGTH, fuck);
					if (ob.st != 100 || ob.end != 101)
					{
						cout << "\n sending the wrong data from the client side";
					}
					int bytes = send(sending_socket, buffer + sent_bytes, bytesToSend, 0);
					if (bytes < 1)
					{
						//cout << "\n cannot send bytes to the client==>" << GETSOCKETERRNO();
					}
					else
					{
						sent_bytes += bytes;

						auto now = std::chrono::system_clock::now();
						auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
						auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
						auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
						auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

						//cout << "\n sent data to the client at==> " <<
						  // hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
					}
				}
			}
			auto now = std::chrono::system_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
			auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
			auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
			auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

			//cout << "\n sent data to the server packet id==>" <<ob.packet_id<<" at=>"<<
			 // hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
			//cout << "\n sent bytes to==>" <<data[i].first;

			count++;
		}
	}
}
bool find_id(deque<upgrade_data>& arr, int id)
{
	for (auto it : arr)
	{
		if (it.id == id)
		{
			return false;
		}

	}
	return true;
}
bool find_bullet_id(deque<bullet_data>& arr, int id)
{
	for (auto it : arr)
	{
		if (it.bullet_id == id)
		{
			return false;
		}
	}
	return true;
}
void graphics::callable_client(int ship_id, Mutex* mutx, int code[rows][columns], Map& map_ob, int peer_s, ship& player)
{

	SOCKET peer_socket = peer_s;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	Control nm;

	deque<ship*>& pl1 = nm.getShipList(2369);

	List<Greed::cannon>& cannon_list = nm.getCannonList(2369);
	//leaving the opaque coordinates list, if needed later it will be imported
	
	double elapsed_time = 0;
	int c = 0;
	double t2 = 0;
	int frames = 0;
	bool gameOver = false;
	//starting the game loop that will sync the algorithm execution with the gameplay
	int cur_frame = -1;//variable to maintain the frame rate of the while loop
	int next_frame = 0;
	control1 control_ob;
	//THIS code is for updating the fuel of the ship

	GameState data;
	long total_time = 0;
	long double frame_number = -1;//this is for packet id that the client will send to the server

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	fd_set master_read;
	FD_ZERO(&master_read);
	FD_SET(peer_socket, &master_read);

	fd_set master_write;
	FD_ZERO(&master_write);
	FD_SET(sending_socket, &master_write);


	fd_set reads;
	fd_set writes;
	FD_ZERO(&reads);
	FD_ZERO(&writes);

	vector<Greed::bullet> newBullets;
	cout << "\n from now on the statements will be printed of your algorithm..\n";
	thread t(GreedMain, ref(player));
	t.detach();
	int frame_no = 0;
	double avg_nav_req = 0;
	int total_frames = 0;
	int tot = 0;
	int frame_rate = 0;
	double check_time = 0;

	int cc = 0;
	double avg_recv = 0;
	double avg_send = 0;
	double avg_processing = 0;
	int prev_pack = 0;

	int previous_packet = -1;
	//accepting the testing message from the servser

	int send_count = 0;
	double avg_send_count = 0;
	int total_frames1 = 0;

	int threshold_health = -1;
	int threshold_ammo = -1;
	int threshold_fuel = -1;
	//to check if the values of these things have changed or not
	int nav_data_count = 0;
	int nav_data_once = 0;
	int b_data_count = 0;
	int b_data_once = 0;
	int c_data_count = 0;
	int c_data_once = 0;
	int u_data_once = 0;
	int u_data_count = 0;

	thread t1(&data_recver, peer_socket, mutx);//starting the thread which will receive the data from the server
	t1.detach();

	thread t3(data_send, mutx, std::ref(pl1));//starting the thread which will send the data to the server
	t3.detach();
	int avg_input = 0;
	int total_count = 0;

	int current_health = pl1[ship_id]->getCurrentHealth();
	int current_fuel = pl1[ship_id]->getCurrentFuel();
	int current_ammo = pl1[ship_id]->getCurrentAmmo();
	
	double ep = 0;
	int prev_tick = 0;
	int effective_frame_rate = 0;
	int wait_tick = 0;
	control1 cont;//object of control1 class

	int nav_res_count = 1;//to keep the count in order to wait before resending anything.
	int bullet_res_count = 1;

	int no_bullet_resend = 0;//to keep a count on number of bullets that were resent

	int no_of_bullets = 0;
	int no_of_times = 0;
	while (1)
	{
		/*NOTES:
		* basic architecture of the game loop for client_v1 is:
		* recv the data from the server( this data will have the game state for that frame and information about other players)
		* process/prepare the data (prepare your own state)
		* send the data (data of your ship) to the server
		*
		* select will return irrespective of the fact that there is any data to read or not.
		* if the connection is interrupted...between the server and client. the client will go on calculating its own state from a static world(since there is no incoming data)
		* but the disconnected client will not move in the game(since there is no connection and for server this client is static)
		* if the connection resumes. the client will not start from where it left, but it will keep on executing instructions since data is not transmitted
		* it will look like magic. the cleint may be dead by the time it gets the connection back.
		* the states will not be consistent in such a case
		*
		*/
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		
		
		if (ep > 1)
		{
			//cout << "\n effective frame rate=>" << effective_frame_rate;
			frame_rate = 0;
			effective_frame_rate = 0;
			ep = 0;
		}
		//next_frame = ep * 60;
		if (next_frame != cur_frame)//under this frame rate is stable
		{


			frame_rate++;
			cc++;
			reads = master_read;
			writes = master_write;

			cur_frame = next_frame;
			//for handling the ship
			total_time = game_tick;

			frames++;
			frame_no++;//for nav data
			tot++;
			//we will wait for 50 microseconds for the select to return
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 50;
			//select(peer_socket + 1, &reads, 0, 0, &timeout);
			recv_data data1;
			int bytes_recv = -1;
			if (pl1[ship_id]->died == 1)
			{
				//cout << "\n breaking because the ship is dead.";
				break;
			}
			


			unique_lock<mutex> lock(mutx->recv_terminal);
			int gone = 0;
			mutx->cond_input_terminal.wait(lock, [&] {
				if (input_data.size() == 0)
				{
					/*
					wait_tick++;
					if (wait_tick == 100)
					{
						wait_tick = 0;
						//game_tick++;
						cout << "\n used";
						return true;
					}
					*/

					return false;
				}
				return true;
				});
			//recving the data from the server here
			if (input_data.size() == 1)//case when only one packet is there from the server
			{

				avg_input += input_data.size();
				gone = 1;
				recv_data data1 = input_data[0];
				input_data.pop_front();
				lock.unlock();

				game_tick = data1.packet_id;
				//cout << "\n recved packet with id=>" << data1.packet_id;
				//cout << "\n packet id==>" << data1.packet_id;
				//cout << "\n packet id=>" << data1.packet_id;
				control_ob.packet_to_pl(data1.shipdata_exceptMe, data1.s1, ship_id, pl1);
				control_ob.packet_to_me(data1.shipdata_forMe, ship_id, pl1);
				//	cout << "\n health is==>" << data1.shipdata_forMe.health;

				previous_packet = data1.packet_id;
				gameOver = data1.gameOver;
				auto now = std::chrono::system_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
				auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
				auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

				//cout << "\n recved data from the server terminal =>" << data1.packet_id << " at the time==> " <<
					//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
			}
			else//enters here only when the size of input data is more than one, then merging is needed
			{
				//taking down all the packets and starting the merging
				deque<recv_data> temp_recv;
				for (int i = 0; i < input_data.size(); i++)
				{
					temp_recv.push_back(input_data[i]);
				}
				input_data.clear();
				lock.unlock();

				game_tick = temp_recv[temp_recv.size() - 1].packet_id;//setting the packet id to the latest packet in the queue
				//starting to unfolding the packet
				control_ob.packet_to_pl(temp_recv[temp_recv.size() - 1].shipdata_exceptMe, temp_recv[temp_recv.size() - 1].s1, ship_id, pl1);//for this just send the latest packet that we have

				control_ob.packet_to_me(temp_recv[temp_recv.size() - 1].shipdata_forMe, ship_id, pl1);//here we have to merge some data
				//adding the previous data only in hit_bullet
				for (int i = temp_recv.size() - 2; i >= 0; i--)
				{
					for (int j = 0; j < temp_recv[i].shipdata_forMe.size_hit_bullet; j++)
					{
						Greed::bullet b;
						cont.data_to_bullet(b, temp_recv[i].shipdata_forMe.hit_bullet[i]);

						pl1[ship_id]->hit_bullet.push_front(b);
					}
				}
			}
			//check if navigation and firing commands reached the server properly, if not then resend them

			for (int i = 0; i < resend_navigation.size(); i++)
			{

				pl1[ship_id]->nav_data_final.push_back(resend_navigation[i]);
			}

			unique_lock<mutex> pk(mutx->game_tick_mutex_client);
			if (pl1[ship_id]->current_count >= 30)
			{
				pl1[ship_id]->prev_count = 0;
				//pl1[ship_id]->prev_count -= pl1[ship_id]->current_count;
				pl1[ship_id]->current_count = 0;
			}
			pl1[ship_id]->current_count++;

			pk.unlock();

			for (int i = 0; i < resend_bullet.size() && i < 5; i++)
			{
				unique_lock<mutex> lk(mutx->m[ship_id]);
				//cout << resend_bullet[i].first.bullet_id << " ";
				//cout << "\n server fire=>" << pl1[ship_id]->server_fire << " " << " client fire=>" << pl1[ship_id]->client_fire;
				if (find_bullet_id(pl1[ship_id]->bullet_info, resend_bullet[i].first.bullet_id) && pl1[ship_id]->client_fire > pl1[ship_id]->server_fire)
				{
					//	cout << "\n reattempting fire command for=>" << resend_bullet[i].first.bullet_id;

					pl1[ship_id]->bullet_info.push_front(resend_bullet[i].first);
					resend_bullet[i].second++;
					no_bullet_resend++;

				}

				else if (pl1[ship_id]->client_fire < pl1[ship_id]->server_fire)
				{
					//	cout << "\n server fire=>" << pl1[ship_id]->server_fire << " " << " client fire=>" << pl1[ship_id]->client_fire;
				}
			}
			bullet_res_count = 1;

			if (resend_navigation.size() > 0)
			{
				nav_res_count++;
			}
			if (resend_bullet.size() > 0)
			{
				bullet_res_count++;
			}
			if (pl1[ship_id]->upgrade_queue.size() > 0)
				//cout << "\n upgrade queue is==>";
				for (int i = 0; i < pl1[ship_id]->upgrade_queue.size(); i++)
				{


					//check if it has to be resent or not?

					if (find_id(pl1[ship_id]->udata, pl1[ship_id]->upgrade_queue[i].udata.id))//in this case retrasmit, without deleting it from the queue
					{
						pl1[ship_id]->udata.push_back(pl1[ship_id]->upgrade_queue[i].udata);
						//cout << "\n reattempting fire with the id=>" << pl1[ship_id]->upgrade_queue[i].udata.id;
					}

					//	cout << pl1[ship_id]->upgrade_queue[i].type << " ";


				}


			unique_lock<mutex> lk1(pl1[ship_id]->mutx->m[ship_id]);
			if (current_health < pl1[ship_id]->health)
			{
				pl1[ship_id]->lock_health = 0;
				//cout << "\n lock unlocked manually for health";

			}
			if (current_ammo < pl1[ship_id]->ammo)
			{
				pl1[ship_id]->lock_ammo = 0;
				//cout << "\n lock unloced manually for ammo";
			}
			if (current_fuel < pl1[ship_id]->fuel)
			{
				pl1[ship_id]->lock_fuel = 0;
			}
			current_health = pl1[ship_id]->health;
			current_ammo = pl1[ship_id]->ammo;
			current_fuel = pl1[ship_id]->fuel;

			lk1.unlock();


			

			//std::time_t result = std::time(nullptr);
			//cout << "\n----------------------------------------------------------";
			//cout << "\n time=>"<<std::localtime(&result)->tm_hour<<":"<< std::localtime(&result)->tm_min<<":"<< std::localtime(&result)->tm_sec << " client frame = >" << total_time << " " << " received frame = >" << data1.packet_id;
			if (gameOver || pl1[ship_id]->died == 1)
			{
				//cout << "\n breaking because the game is over(as told by the server, or the ship has died)";
				break;
			}
			if (!gameOver)
			{
				//using nav_data
				
				unique_lock<mutex> sigma(mutx->m[ship_id]);
				if (pl1[ship_id]->nav_data.size() > 0)
				{
					for (int i = 0; i < pl1[ship_id]->nav_data.size(); i++)
					{
						pl1[ship_id]->nav_data_final.push_back(pl1[ship_id]->nav_data[i]);
						//									cout << "\n sending navigation request to the server at==============================>" << game_tick;
					}
					pl1[ship_id]->nav_data.clear();
				}
				sigma.unlock();

				//getPointPath has to be protected by a mutex

				//here we are starting to manage the events
				//checking and deleting events from passive_event queue
				// /*
				// here we will delete all the events which are not happening
				//checking for event1

				if (mutx->event_mutex[ship_id].try_lock())
				{
					//deleting from the current_event

					pl1[ship_id]->current_event_point = 0;
					for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
					{
						//checking for event1
						if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::ShipsInMyRadius)
						{
							//checking if this event is still happening, if not then delete this event
							bool check = false;
							vector<int> l = pl1[ship_id]->shipsInMyRadius();
							if (l.size() == pl1[ship_id]->passive_event[j].radiusShip.sid.size())
							{
								for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
								{
									if (l[k] != pl1[ship_id]->passive_event[j].radiusShip.sid[k])
									{
										check = true;
										break;


									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}

						}
						//checking for next event
						//event2
						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInShipRadius)
						{
							bool check = false;
							vector<int> l = pl1[ship_id]->shipsIamInRadiusOf();
							if (l.size() == pl1[ship_id]->passive_event[j].radiusShip.sid.size())
							{
								for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
								{
									if (l[k] != pl1[ship_id]->passive_event[j].radiusShip.sid[k])
									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}

						}
						//checking for event3
						else	if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInCannonRadius)
						{
							bool check = false;
							vector<int> l = pl1[ship_id]->cannonsIamInRadiusOf();
							if (l.size() == pl1[ship_id]->passive_event[j].radiusCannon.cid.size())
							{


								for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
								{

									if (l[k] != pl1[ship_id]->passive_event[j].radiusCannon.cid[k])

									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event4
						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::CannonsInMyRadius)
						{
							bool check = false;
							vector<int> l = pl1[ship_id]->cannonsInMyRadius();
							if (l.size() == pl1[ship_id]->passive_event[j].radiusCannon.cid.size())
							{
								for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
								{
									if (l[k] != pl1[ship_id]->passive_event[j].radiusCannon.cid[k])
									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event7
						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowHealth)
						{

							if (pl1[ship_id]->getCurrentHealth() > pl1[ship_id]->threshold_health)
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event8 
						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoAmmo)
						{
							if (pl1[ship_id]->getCurrentAmmo() > 0)
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event9
						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowAmmo)
						{
							if (pl1[ship_id]->getCurrentAmmo() > pl1[ship_id]->threshold_ammo)

							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event10

						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoFuel)
						{
							if (pl1[ship_id]->getCurrentFuel() > 0)
							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event 11
						else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowFuel)
						{
							if (pl1[ship_id]->getCurrentFuel() > pl1[ship_id]->threshold_fuel)

							{
								auto it = pl1[ship_id]->passive_event.begin();
								advance(it, j);
								pl1[ship_id]->passive_event.erase(it);
								j--;
							}
						}

					}
					mutx->event_mutex[ship_id].unlock();

				}

				//event handling starts here


					//handling events for all the ships
				if (mutx->event_mutex[ship_id].try_lock())
				{
					//important info: here ids are not given to the events, they will be given only after their validation

					//adding events in the queue of the respective ships..
					//checking for shipsInMyRadius event first
					vector<int> l1 = pl1[ship_id]->shipsInMyRadius();
					Event e1;
					//make all the events first then check it with passive_event in a single loop
					if (l1.size() > 0)//it means that event has happened
					{
						e1.initialize(total_time, Event::EventType::ShipsInMyRadius, pl1[ship_id]->ship_id);

						e1.radiusShip = Event::Radius_ship(l1);

						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							//chekcing for event1
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::ShipsInMyRadius && e1.eventType == Event::EventType::ShipsInMyRadius)
							{
								//cout << "\n in event0==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp) <= 5 && pl1[ship_id]->passive_event[j].radiusShip.sid.size() == e1.radiusShip.sid.size())//event occuring in intervals of three frames
								{
									//if these conditions are satisfied then only check equality operation between the events
									gotcha = 1;
									int found = 0;
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
									{
										if (!(pl1[ship_id]->passive_event[j].radiusShip.sid[k] == e1.radiusShip.sid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 0)//this means that events are same
									{
										pl1[ship_id]->passive_event[j].ttl = e1.timestamp;//updating the last occuring of the event in the passive event queue
										break;
									}
									else if (found == 1)//this is a new event
									{
										e1.setEventId();
										pl1[ship_id]->current_event.push_back(e1);//added the event in the current_event
										pl1[ship_id]->passive_event.push_back(e1);//added the event in the passive_event
										//cout << "\n pushing the event that a ship is in my radius...";
										break;
									}
								}


							}
						}
						if (gotcha == 0)
						{

							e1.setEventId();
							pl1[ship_id]->current_event.push_back(e1);//added the event in the current_event
							pl1[ship_id]->passive_event.push_back(e1);//added the event in the passive_event

						}
					}
					//checking for IamInShipRadius

					Event e2;
					vector<int> l2 = pl1[ship_id]->shipsIamInRadiusOf();
					if (l2.size() > 0)
					{
						e2.initialize(total_time, Event::EventType::IamInShipRadius, pl1[ship_id]->ship_id);
						e2.radiusShip = Event::Radius_ship(l2);
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInShipRadius && e2.eventType == Event::EventType::IamInShipRadius)
							{
								//cout << "\n in event1==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[ship_id]->passive_event[j].ttl - e2.timestamp) <= 5 && pl1[ship_id]->passive_event[j].radiusShip.sid.size() == e2.radiusShip.sid.size())//event occuring in intervals of three frames
								{
									//if these conditions are satisfied then only check equality operation between the events
									gotcha = 1;
									int found = 0;
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
									{
										if (!(pl1[ship_id]->passive_event[j].radiusShip.sid[k] == e2.radiusShip.sid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 0)//this means that events are same
									{
										//cout << "\n i worked";
										pl1[ship_id]->passive_event[j].ttl = e2.timestamp;//updating the last occuring of the event in the passive event queue
										break;
									}
									else if (found == 1)//this is a new event
									{
										e2.setEventId();
										pl1[ship_id]->current_event.push_back(e2);//added the event in the current_event
										pl1[ship_id]->passive_event.push_back(e2);//added the event in the passive_event
										break;
									}
								}
								else if (e2.eventType == Event::EventType::IamInShipRadius)
								{
									e2.setEventId();
									pl1[ship_id]->current_event.push_back(e2);//added the event in the current_event
									pl1[ship_id]->passive_event.push_back(e2);//added the event in the passive_event
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e2.setEventId();
							pl1[ship_id]->current_event.push_back(e2);//added the event in the current_event
							pl1[ship_id]->passive_event.push_back(e2);//added the event in the passive_event
						}

					}
					//making for the event IamInCannonRadius
					Event e3;
					vector<int> l3 = pl1[ship_id]->cannonsIamInRadiusOf();
					if (l3.size() > 0)
					{
						e3.initialize(total_time, Event::EventType::IamInCannonRadius, pl1[ship_id]->ship_id);

						e3.radiusCannon = Event::Radius_cannon(l3);
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInCannonRadius && e3.eventType == Event::EventType::IamInCannonRadius)
							{
								//cout << "\n in event2==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[ship_id]->passive_event[j].ttl - e3.timestamp) <= 3 && pl1[ship_id]->passive_event[j].radiusCannon.cid.size() == e3.radiusCannon.cid.size())
								{
									gotcha = 1;
									int found = 0;
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
									{
										if (!(pl1[ship_id]->passive_event[j].radiusCannon.cid[k] == e3.radiusCannon.cid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 1)//the events are not equal this is a new event
									{
										e3.setEventId();
										pl1[ship_id]->current_event.push_back(e3);
										pl1[ship_id]->passive_event.push_back(e3);
										break;

									}
									else if (found == 0)//event is same
									{
										pl1[ship_id]->passive_event[j].ttl = e3.timestamp;
										break;
									}
								}
								else if (e3.eventType == Event::EventType::IamInCannonRadius)
								{
									e3.setEventId();
									pl1[ship_id]->current_event.push_back(e3);
									pl1[ship_id]->passive_event.push_back(e3);
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e3.setEventId();
							pl1[ship_id]->current_event.push_back(e3);
							pl1[ship_id]->passive_event.push_back(e3);
						}

					}
					//making of the event CannonsInMyRadius
					Event e4;
					vector<int> l4 = pl1[ship_id]->cannonsInMyRadius();
					if (l4.size() > 0)
					{
						e4.initialize(total_time, Event::EventType::CannonsInMyRadius, pl1[ship_id]->ship_id);
						e4.radiusCannon = Event::Radius_cannon(l4);
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::CannonsInMyRadius && e4.eventType == Event::EventType::CannonsInMyRadius)
							{
								//cout << "\n in event3==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[ship_id]->passive_event[j].ttl - e4.timestamp) <= 3 && pl1[ship_id]->passive_event[j].radiusCannon.cid.size() == e4.radiusCannon.cid.size())
								{
									int found = 0;
									gotcha = 1;
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
									{
										if (!(pl1[ship_id]->passive_event[j].radiusCannon.cid[k] == e4.radiusCannon.cid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 1)//the events are not equal this is a new event
									{
										e4.setEventId();
										pl1[ship_id]->current_event.push_back(e4);
										pl1[ship_id]->passive_event.push_back(e4);
										break;

									}
									else if (found == 0)//event is same
									{
										pl1[ship_id]->passive_event[j].ttl = e4.timestamp;
										break;
									}
								}
								else if (e4.eventType == Event::EventType::CannonsInMyRadius)
								{
									e4.setEventId();
									pl1[ship_id]->current_event.push_back(e4);
									pl1[ship_id]->passive_event.push_back(e4);
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e4.setEventId();
							pl1[ship_id]->current_event.push_back(e4);
							pl1[ship_id]->passive_event.push_back(e4);
						}
					}
					//making of the event ShipFire

					Event e5;
					unordered_map<int, Greed::bullet> l5;
					for (int j = 0; j < pl1[ship_id]->bullet_hit_tempo.size(); j++)
					{
						if (pl1[ship_id]->bullet_hit_tempo[j].launch_ship != -1)
						{

							l5.insert(pair<int, Greed::bullet>(pl1[ship_id]->bullet_hit_tempo[j].launch_ship, pl1[ship_id]->bullet_hit_tempo[j]));
							auto it = pl1[ship_id]->bullet_hit_tempo.begin();
							advance(it, j);
							pl1[ship_id]->bullet_hit_tempo.erase(it);
							j--;


						}
					}
					if (l5.size() > 0)//this event is set here
					{
						e5.initialize(total_time, Event::EventType::ShipFire, pl1[ship_id]->ship_id);
						e5.setEventId();
						e5.shipFire = Event::ShipFire(l5);
						pl1[ship_id]->current_event.push_back(e5);
					}

					//making of the event cannonFire
					Event e6;
					unordered_map<int, Greed::bullet> l6;
					for (int j = 0; j < pl1[ship_id]->bullet_hit_tempo.size(); j++)
					{
						if (pl1[ship_id]->bullet_hit_tempo[j].launch_cannon != -1)
						{
							l6.insert(pair<int, Greed::bullet>(pl1[ship_id]->bullet_hit_tempo[j].launch_cannon, pl1[ship_id]->bullet_hit_tempo[j]));
							auto it = pl1[ship_id]->bullet_hit_tempo.begin();
							advance(it, j);
							pl1[ship_id]->bullet_hit_tempo.erase(it);
							j--;

						}

					}
					if (l6.size() > 0)
					{
						e6.initialize(total_time, Event::EventType::CannonFire, pl1[ship_id]->ship_id);
						e6.setEventId();

						e6.cannonFire = Event::CannonFire(l6);
						pl1[ship_id]->current_event.push_back(e6);
						//hey
					}
					//making of the event LowHealth
					Event e7;

					if (pl1[ship_id]->getCurrentHealth() < pl1[ship_id]->threshold_health)//there will be a health threshold that can be set by every player
					{
						e7.initialize(total_time, Event::EventType::LowHealth, pl1[ship_id]->ship_id);
						e7.lowHealth = Event::LowHealth(pl1[ship_id]->getCurrentHealth());
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowHealth && e7.eventType == Event::EventType::LowHealth)
							{
								gotcha = 1;
								if (abs(pl1[ship_id]->passive_event[j].ttl - e7.timestamp) <= 3)//the event need not be repeated
								{
									pl1[ship_id]->passive_event[j].ttl = e7.timestamp;//updating the timestamp
									pl1[ship_id]->passive_event[j].lowHealth.health = e7.lowHealth.health;//updating the new health
								}
								else
								{
									e7.setEventId();
									pl1[ship_id]->passive_event.push_back(e7);
									pl1[ship_id]->current_event.push_back(e7);
									break;


								}
							}
						}
						if (gotcha == 0)
						{
							e7.setEventId();
							pl1[ship_id]->passive_event.push_back(e7);
							pl1[ship_id]->current_event.push_back(e7);
						}
					}
					//making of event NoAmmo
					Event e8;
					if (pl1[ship_id]->getCurrentAmmo() <= 0)
					{
						e8.initialize(total_time, Event::EventType::NoAmmo, pl1[ship_id]->ship_id);
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoAmmo)
							{
								if (abs(pl1[ship_id]->passive_event[j].ttl - e8.timestamp) <= 5)
								{
									pl1[ship_id]->passive_event[j].ttl = e8.timestamp;
									gotcha = 1;
									break;
								}
							}
						}
						if (gotcha == 0)//this is a new event
						{
							e8.setEventId();
							pl1[ship_id]->passive_event.push_back(e8);
							pl1[ship_id]->current_event.push_back(e8);

						}

					}
					//making of the event LowAmmo
					Event e9;
					if (pl1[ship_id]->getCurrentAmmo() < pl1[ship_id]->threshold_ammo)
					{
						e9.initialize(total_time, Event::EventType::LowAmmo, pl1[ship_id]->ship_id);
						e9.lowAmmo = Event::LowAmmo(pl1[ship_id]->getCurrentAmmo());
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowAmmo)

							{
								if (abs(pl1[ship_id]->passive_event[j].ttl - e9.timestamp) <= 5)
								{
									gotcha = 1;
									pl1[ship_id]->passive_event[j].ttl = e9.timestamp;
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e9.setEventId();
							pl1[ship_id]->passive_event.push_back(e9);
							pl1[ship_id]->current_event.push_back(e9);
						}
					}

					//making of the event NoFuel
					Event e10;
					if (pl1[ship_id]->getCurrentFuel() <= 0)
					{
						e10.initialize(total_time, Event::EventType::NoFuel, pl1[ship_id]->getShipId());
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoFuel)

							{
								if (abs(pl1[ship_id]->passive_event[j].ttl - e10.timestamp) <= 5)
								{
									gotcha = 1;
									pl1[ship_id]->passive_event[j].ttl = e10.timestamp;
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e10.setEventId();
							pl1[ship_id]->passive_event.push_back(e10);
							pl1[ship_id]->current_event.push_back(e10);
						}

					}
					//making of the event LowFuel
					Event e11;
					if (pl1[ship_id]->getCurrentFuel() < pl1[ship_id]->threshold_fuel)
					{
						e11.initialize(total_time, Event::EventType::LowFuel, pl1[ship_id]->getShipId());
						e11.lowFuel = Event::LowFuel(pl1[ship_id]->getCurrentFuel());
						int gotcha = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowFuel)

							{
								if (abs(pl1[ship_id]->passive_event[j].ttl - e11.timestamp) <= 5)
								{
									gotcha = 1;
									pl1[ship_id]->passive_event[j].ttl = e11.timestamp;
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e11.setEventId();
							pl1[ship_id]->passive_event.push_back(e11);
							pl1[ship_id]->current_event.push_back(e11);
						}
					}
					//now checking which ones of them are in passive_event queue and new events will be added in both the queues

					//adding the event of colliding of ships
					Event e12;

					if (pl1[ship_id]->collided_ships.size() > 0)//if the event of collision has happend
					{
						//cout << "\n adding to the event";
						e12.initialize(total_time, Event::EventType::ShipCollision, pl1[ship_id]->ship_id);
						e12.setEventId();
						e12.shipCollision = Event::ShipCollision(pl1[ship_id]->collided_ships);
						pl1[ship_id]->current_event.push_back(e12);
						pl1[ship_id]->collided_ships.clear();

					}




					mutx->event_mutex[ship_id].unlock();
				}


				/*event handling is over here*/

				
			}

			//send the data over here
			
			if (pl1[1]->isShipInMyRadius(0))
			{
				//cout << "\n the ship is in my radius at the game tick==>" << game_tick;
				auto now = std::chrono::system_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
				auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
				auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

				//cout <<  " at the time==> " <<
				//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
			}

			if (pl1[ship_id]->died == 0)//send the data only if the ship is alive
			{
				send_data data2;
				shipData_forServer shipdata;
				std::memset((void*)&shipdata, 0, sizeof(shipdata));
				std::memset((void*)&data2, 0, sizeof(data2));


				//if (threshold_health==pl1[ship_id]->threshold_health&&threshold_ammo==pl1[ship_id]->threshold_ammo&&threshold_fuel==pl1[ship_id]->threshold_fuel&& pl1[ship_id]->nav_data_final.size() == 0 && pl1[ship_id]->bullet_info.size() == 0 && pl1[ship_id]->udata.size() == 0 && pl1[ship_id]->map_cost_data.size() == 0)

				threshold_ammo = pl1[ship_id]->threshold_ammo;
				threshold_fuel = pl1[ship_id]->threshold_fuel;
				threshold_health = pl1[ship_id]->threshold_health;
				for (int i = 0; i < pl1[ship_id]->bullet_info.size() && i < 15; i++)
				{
					//cout << "\n sending firing data at==>" << game_tick;
				}
				for (int i = 0; i < pl1[ship_id]->nav_data_final.size(); i++)
				{
					//cout << "\n sending navigation data at==>" << game_tick;
				}

				no_of_bullets += pl1[ship_id]->bullet_info.size();
				no_of_times++;
				control_ob.mydata_to_server(pl1, ship_id, shipdata, newBullets, mutx, nav_res_count, bullet_res_count, no_bullet_resend);



				frame_number++;
				game_tick++;
				effective_frame_rate++;
				//cout << "\n in callable sending the packet==>" << game_tick;
				data2.packet_id = game_tick;
				data2.shipdata_forServer = shipdata;
				data2.user_cred = user_credentials(username, password);
				for (int i = 0; i < data2.shipdata_forServer.size_upgrade_data; i++)
				{
					if (data2.shipdata_forServer.udata[i].type == 1)
					{
						//cout << "\n sending for health to the sever at the health level=>" << pl1[ship_id]->getCurrentHealth();
					}
				}
				if (data2.shipdata_forServer.size_navigation > 0 && my_id == 1)
				{
					//cout << "\n sent navigation request to the server==>"<<game_tick;
					auto now = std::chrono::system_clock::now();
					auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
					auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
					auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
					auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

					//cout << "\n at the time==> " <<
					//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
				}
				send_count++;
				unique_lock<mutex> lk(mutx->send_terminal);
				terminal_data.push_back(data2);
				lk.unlock();
				mutx->cond_terminal.notify_one();


			}
			
			total_count++;

		}

		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - begin);
		elapsed_time += time_span.count();
		if (elapsed_time > 1)
		{
			elapsed_time = 0;
			//cout << "\n frame rate is==>" << frame_rate;
			//frame_rate = 0;
			total_frames1++;
		}

		next_frame = elapsed_time * 80;


	}
	cout << "\n number of bullets coming per time is==>" << (double)no_of_bullets / no_of_times;
	CLOSESOCKET(peer_socket);
	CLOSESOCKET(sending_socket);

	WSACleanup();

	while (1)
	{
	}

}
std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

string input_password()
{
	const int MAX_PASSWORD_LENGTH = 20;
	char password1[MAX_PASSWORD_LENGTH + 1]; // +1 for null terminator
	const char BACKSPACE = 8;
	const char ENTER = 13;
	std::cout << "Enter your password==>";
	char ch;
	int i = 0;
	while (true) {
		ch = _getch();

		if (ch == ENTER) {
			password1[i] = '\0'; // Null-terminate the password
			break;
		}
		else if (ch == BACKSPACE) {
			if (i > 0) {
				std::cout << "\b \b"; // Move cursor back, overwrite with space, move back again
				--i;
			}
		}
		else if (i < MAX_PASSWORD_LENGTH - 1) {
			std::cout << '*';
			password1[i++] = ch;
		}
	}

	return string(password1);
}
string checkEmail()
{
	string email;
	bool true_mail = false;
	while (!true_mail)
	{
		cin >> email;
		std::regex pattern("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

		// Check if the email matches the pattern
		true_mail = std::regex_match(email, pattern);
		if (!true_mail)
		{
			cout << "\n please enter a valid email==>";
		}
	}
	return email;
	//now checking the email

}
bool login_function(SOCKET lobby_socket);
void createAccount(SOCKET lobby_socket)
{
	int status_code = 0;
	cout << "\n You don't seem to have an account with us...let's create you one...\n Enter the details:\n what do you want to be called? =>";
	while (status_code != 1)
	{
		cin >> username;
		//checking if this username is available or not
		user_credentials ob(2, username, "");
		int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
		bytes = recv(lobby_socket, (char*)&status_code, sizeof(status_code), 0);
		if (status_code == -1)
		{
			cout << "\n Oops..This username is taken, try something else? Enter another username=>";
		}

	}
	//now we are having our username set
	cout << "\n Wow! You have picked youself a deserving name...\n";
	password = input_password();
	cout << "\n Just a few more things...\n Enter you email address ( Enter a valid email, because all your in-game credits and rewards will be intimidated to you on that email)=>";
	string email = checkEmail();
	cout << "\n which country do you come from?";
	string country;
	cin >> country;
	cout << "\n what is your designation? eg. student, working etc.==>";
	string role;
	cin >> role;
	user_credentials ob(1, username, password, email, role, country);
	send(lobby_socket, (char*)&ob, sizeof(ob), 0);
	login_function(lobby_socket);

}
bool login_function(SOCKET lobby_socket)
{
	cout << "\n enter you username==>";
	cin >> username;
	password = input_password();
	user_credentials ob(0, username, password, tournament_id);
	int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
	if (bytes < 0)
	{
		cout << "\n could not send the credentials to the lobby server=>" << GetLastErrorAsString();
	}
	int status_code = 0;
	bytes = recv(lobby_socket, (char*)&status_code, sizeof(status_code), 0);
	//if everything goes right the status code should be 1
	if (bytes < 1)
	{
		cout << "\n error in recving bytes==>" << GETSOCKETERRNO();
	}
	cout << "\n the status code is==>" << status_code;
	while (status_code != 1)
	{
		if (status_code == 2)
		{
			//the password of the user is wrong, so promt the user to input the password again
			cout << "\n the password you entered is wrong...";
			password = input_password();
			user_credentials ob(0, username, password);
			int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
			status_code = 0;
			bytes = recv(lobby_socket, (char*)&status_code, sizeof(status_code), 0);

		}
		if (status_code == -1)
		{
			createAccount(lobby_socket);
		}
		if (status_code == 3)
		{
			cout << "\n" << username << " is already logged in with us! please try to log in with a different username...";
		}
	}
	return true;

}
int connect_to_lobby_server()
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* bind_address;
	getaddrinfo(ip_address.c_str(), "8080", &hints, &bind_address);
	lobby_socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(lobby_socket))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		//return 1;
	}

	while (connect(lobby_socket, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		cout << GetLastErrorAsString();
		//return 1;
	}
	freeaddrinfo(bind_address);
	cout << "\n connected with the lobby server..";
	//sending hi to the server
	//sending my credentials to the server
	cout << "\n Do you a specific tournament ID?\n press 1 if you have it, else press 2==>";
	int have;
	cin >> have;
	if (have == 1)
	{
		cout << "\n enter the tournament id==>";
		cin >> tournament_id;
	}
	else
	{
		tournament_id = "RANKED";
	}
	cout << "\n Enter 1 to login ( if you have a username and password) \n Enter 2 to signup (if you don't have any username)";
	int choice = 0;
	cin >> choice;
	if (choice == 1)//the user wants to login
	{
		login_function(lobby_socket);


	}
	else if (choice == 2)
	{
		createAccount(lobby_socket);
	}






	cout << "\n waiting for game token and port of the game server...";

	server_startup start;
	int port;
	int bytes = recv(lobby_socket, (char*)&start, sizeof(start), 0);
	if (bytes < 1)
	{
		cout << "\n error in recv bytes from lobby server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	port = start.port;
	game_token = start.token;
	cout << "\n received port and token from the lobby server..";
	return port;
}
void heartbeat_lobby()
{
	//sending heartbeat messages to the lobby server
	double elapsed_time = 0;
	while (1)
	{
		Sleep(1000);
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();

		//sending heartbeat messages to the server here..............
		//chekcing if one second has elapsed or not 
		//these heartbeat message are used to  tell the server that the client is active so dont close the tcp connection
		//these heartbeat messages are set to be sent every 1 second of the tick
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - begin);
		elapsed_time += time_span.count();
		if (elapsed_time > 10)
		{
			elapsed_time = 0;
			//send the heartbeat message here
			int beat = 1;
			int bytes = send(lobby_socket, (char*)&beat, sizeof(beat), 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the byes==>" << GETSOCKETERRNO();
			}
			cout << "\n sent heartbeat to the lobby server...";

		}
	}
}
int main(int argc, char* argv[])
{
	//extracting the data
	cout << "\n this is the version 0.0.1..";

	ip_address = "3.111.6.185";//this is the ip address of the greed_5
	cout << "\n enter the mode in which you want to run this client.. press 1. for client with display unit.\n otherwise press 2==>";
	cin >> mode;
#if defined(_WIN32) 
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		fprintf(stderr, "Failed to initialize.\n");
		cout << "\n winsock not found.";
		return 1;
	}
#endif
	int port = 0;//port is of the game server which the lobby server will return.
	SOCKET socket_listen = 0;
	port = connect_to_lobby_server();
	//sending heartbeat messages to the lobby server
	thread t(heartbeat_lobby);
	t.detach();//starting the thread to send the hearbeat to the lobby server...
	socket_listen = connect_to_server(port);   //no connection is established here only the socket initialization is done here
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	/*
	cout << "\n waiting to recv the data before going for tcp ";
	int st = 0;
	int recvy = recv(socket_listen, (char*)&st, sizeof(st),0);
	if (st != 1)
	{
		cout << "\n found the wrong data";
	}
	*/

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* bind_address;
	int port2 = port + 3;
	char port2_str[10];
	sprintf(port2_str, "%d", port2);
	//cout << "\n port of the server2 is==>" << port2_str;
	getaddrinfo(ip_address.c_str(), port2_str, &hints, &bind_address);
	cout << "\n tcp connection port is=>" << port2_str;
	SOCKET tcp_socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(tcp_socket))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		//return 1;
	}
	int enableKeepAlive = 1;
	setsockopt(tcp_socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&enableKeepAlive, sizeof(enableKeepAlive));
	while (::connect(tcp_socket, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		cout << GetLastErrorAsString();
		//return 1;
	}
	char ip[100];
	char cport[100];
	getnameinfo(bind_address->ai_addr, bind_address->ai_addrlen, ip, sizeof(ip), cport, sizeof(cport), NI_NUMERICHOST | NI_NUMERICSERV);
	cout << "\n address of the tcp server=>" << ip;
	cout << "\n port of the tcp server=>" << cport;
	freeaddrinfo(bind_address);
	cout << "\n tcp connected with the client..";
	//cout << "\n connected with the game server using tcp. ";
	int msg;
	if (mode == 1)
		msg = 0;//0 means client_v1_process
	else if (mode == 2)
	{
		msg = 2;
	}
	greet_client gc;
	gc.code = msg;
	gc.user_cred = user_credentials(username, password, tournament_id);
	gc.isBot = false;
	strcpy(gc.token, game_token.c_str());


	if (!send(tcp_socket, (char*)&gc, sizeof(gc), 0))
	{
		cout << "\n cannot send the code to the server=>" << GETSOCKETERRNO();
	}

	cout << "\n waiting for the game server to send me the id..";
	recv(tcp_socket, (char*)&my_id, sizeof(my_id), 0);
	cout << "\n id sent by the client is=>" << my_id;


	//starting the display unit
	if (mode == 1)
	{
		//added the thing that when the game overs, the client will break the loop and close the connection.
		char path_c[MAX_PATH];
		string path;
		string cwd_path;
		if (GetCurrentDirectoryA(MAX_PATH, path_c) != 0)
		{
			cwd_path = path_c;
			path = path_c;
		}
		path += "\\display_package\\client_v2_new.exe ";
		cout << "\n path of the process to be opened is==>" << path;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		//LPTSTR* arg = { "hello.exe" };
		//convert port to string and append in commandLine

		string port_str = to_string(port);
		string id = to_string(my_id);
		char cwd[256];

		string commandLine = path + port_str + " " + id + " " + username + " " + password + " " + game_token + " " + ip_address;
		//cout << "\n command line is==>" << commandLine;
		char str[500];
		strcpy(str, commandLine.c_str());
		cwd_path += "\\display_package\\";
		//char commandLine[] = "\"F:\\current projects\\GREED(programming game)\\GREED(programming game)\\client_v2_new.exe\" 8080";
		if (!CreateProcessA(NULL, str, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL,cwd_path.c_str(), &si, &pi))
		{
			cout << "Error creating process" << GetLastErrorAsString();

			return 1;
		}
		else
		{
			cout << "Process created";
			//return 0;
		}
	}
	//before stage2 we need to keep on sending heart beat messages to the server to keep the connection alive as long as some data is
	// not ready to recv from the server
	//stage 2.............
	//asking if everything is ready for stage 2?
	fd_set master;
	fd_set read;
	FD_ZERO(&master);
	FD_ZERO(&read);
	FD_SET(tcp_socket, &master);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	double elapsed_time = 0;
	while (1)
	{
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		read = master;
		select(tcp_socket + 1, &read, 0, 0, &timeout);
		if (FD_ISSET(tcp_socket, &read))
		{
			int st = 1;
			int b = recv(tcp_socket, (char*)&st, sizeof(st), 0);
			cout << "\n we are ready for stage 2==>" << st;
			break;
		}
		//sending heartbeat messages to the server here..............
		//chekcing if one second has elapsed or not 
		//these heartbeat message are used to  tell the server that the client is active so dont close the tcp connection
		//these heartbeat messages are set to be sent every 1 second of the tick
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - begin);
		elapsed_time += time_span.count();
		if (elapsed_time > 60)
		{
			elapsed_time = 0;
			//send the heartbeat message here
			char buff[100] = "hi terminal";
			int bytes = send(tcp_socket, buff, sizeof(buff), 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the byes==>" << GETSOCKETERRNO();
			}
			cout << "\nsent heartbeat to the server";

		}
	}


	int bytes = send(tcp_socket, (char*)&my_id, sizeof(my_id), 0);

	if (bytes < 1)
	{
		cout << "\n error in sending my id to the game server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	cout << "\n sent the bytes to the server...";
	Startup_info_client start_data;
	memset((void*)&start_data, 0, sizeof(start_data));
	bytes = recv(tcp_socket, (char*)&start_data, sizeof(start_data), 0);
	if (bytes < 1)
	{
		cout << "\n error in receiving startup info from the game server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	const int no_of_players = start_data.no_of_players;
	cout << "\n number of players==>" << no_of_players;

	ship* player = new ship[no_of_players];

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
	spawn.push_back(start_data.starting_pos);

	player[start_data.ship_id].initialize_player(start_data.ship_id, "hawk", &mutx, silist, code, 5, spawn[0]);//last element is the tile pos of the player

	for (int i = 0; i < no_of_players; i++)
	{
		if (i != start_data.ship_id)
		{
			player[i].mutx = &mutx;
			player[i].ship_id = i;
			player[i].mutex_id = i;
			player[i].shipInfoList = silist;
		}
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

	//waiting for the game server for the green signal to start the game;
	cout << "\n waiting for the server to send green signal to start the game....";
	int start = 0;
	bytes = recv(tcp_socket, (char*)&start, sizeof(start), 0);
	if (bytes < 1)
	{
		cout << "\n bytes recved are==>" << bytes;
		cout << "\n error in receiving startup info from the game server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	cout << "\n starting the game==>" << start;




	//sending the final request to the client....for the udp socket

	for (int i = 1; i <= 10; i++)
	{

		int bytes = send(socket_listen, (char*)&my_id, sizeof(my_id), 0);
		if (bytes < 1)
		{
			cout << "\n error in sending the final bytes==>" << GetLastErrorAsString();
		}
	}
	//sending to the server from my sending socket
	id_port_class obb(my_id, username);
	for (int i = 1; i <= 10; i++)
	{
		int bytes = send(sending_socket, (char*)&obb, sizeof(obb), 0);
		if (bytes < 1)
		{
			cout << "\n error in sending the final bytes==>" << GETSOCKETERRNO();
		}
	}

	cout << "\n waiting for the server to send the final flag to start the game...";
	int flag = 0;
	bytes = recv(tcp_socket, (char*)&flag, sizeof(flag), 0);
	if (bytes < 1)
	{
		cout << "\n cannot recv the final bytes from the server==>" << GetLastErrorAsString();
	}
	if (!flag)
	{
		cout << "\n the value of flag recved is==>" << flag;
		cout << "\n going to while loop....";

	}
	while (!flag)
	{

	}


	CLOSESOCKET(tcp_socket);
	graphics cg;
	cg.callable_client(start_data.ship_id, &mutx, code, map1, socket_listen, player[start_data.ship_id]);
	//waiting for the child process to finish
	if (mode == 1)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		cout << "\n child completed";
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}


}



