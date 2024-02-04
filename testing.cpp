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

	
	while (1)
	{

		if (ob.frame_rate_limiter())
		{//this is anchit rana talking to the world and i want ot know the difference between
			
			
		}

	}



}

