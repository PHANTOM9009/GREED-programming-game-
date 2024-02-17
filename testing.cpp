#include "online_lib2.hpp"

int find_ship_to_kill(deque<shipInfo>& shipList, int myid, ship& ob, int hate_id)
{
	int mini = INT_MAX;
	int index = -1;
	for (int i = 0; i < shipList.size(); i++)
	{
		if (shipList[i].getDiedStatus() == 0 && i != myid && i != hate_id)
		{
			int score = 0.6 * shipList[i].getCurrentHealth() + 0.4 * ob.getDistance(i);
			if (score < mini)
			{
				mini = score;//a heuristic to find the best prey
				index = i;
			}
		}
	}
	return index;

}

void GreedMain(ship& ob)
{
	//setting the aim

	vector<Greed::cannon> clist=ob.getCannonList();
	int dest = 0;
	int i = 0;
	Greed::coords desti = ob.getCurrentTile();
	sf::Clock clock;
	double elapsed_time = 0;
	int frame_rate = 0;
	while (1)
	{
		elapsed_time += clock.restart().asSeconds();
		if (elapsed_time > 1)
		{
			cout << "\n the frame rate of the algorithm is==>" << frame_rate;
			elapsed_time = 0;
			frame_rate = 0;

		}
		if (ob.frame_rate_limiter())

		{//this is anchit rana talking to the world and i want ot know the difference between
			
			frame_rate++;
			if (ob.isShipInMotion()==0 && i<3)
			{
				 desti = ob.getRadiusCoords_cannon(clist[i].getCannonId())[0];
				 cout << "\n entity at==>" << desti.r << " " << desti.c;
				ob.Greed_setPath(desti);
				i++;
				dest = 1;
			}
			for (int j = 0; j < ob.cannonsInMyRadius().size(); j++)
			{
				//cout << "\n cannon in my radius is==>" << ob.cannonsInMyRadius()[j];
			}
			if (ob.getCurrentHealth() <= 10)
			{
				ob.Greed_upgradeHealth(10);
			}
			
		}

	}



}

