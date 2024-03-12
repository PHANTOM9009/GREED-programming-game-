
//#include "greed_offline.hpp"
#include<random>
#include "online_lib2.hpp"

int generateRandomInt(int lowerLimit, int upperLimit) {
	std::random_device rd; // Obtain a random number from hardware
	std::mt19937 eng(rd()); // Seed the generator

	// Define the distribution
	std::uniform_int_distribution<> distr(lowerLimit, upperLimit);

	return distr(eng); // Generate a random number within the specified range
}
	int find_ship_to_kill(deque<shipInfo> shipList, int myid, ship& ob,int hate_id)
	{
		int mini = INT_MAX;
		int index = -1;
		for (int i = 0; i < shipList.size(); i++)
		{
			if (shipList[i].getDiedStatus() == 0 && i != myid)
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
	Greed::coords runAway(ship &ob)
	{
		//generate a random number between 0 and 11
		//generate a random number between 0 and 23
		while (1)
		{
			int r = generateRandomInt(0, 11);
			int c = generateRandomInt(0, 23);
			if (ob.whatsHere(Greed::coords(r, c)).getEntity() == Entity::WATER)
			{
				return Greed::coords(r, c);
			}
		}
		
	}

	void GreedMain(ship& ob)
	{
		//setting the aim
		
		vector <Greed::cannon> cannonList = ob.getCannonList();
		for (int j = 0; j < cannonList.size(); j++)
		{


			vector<Greed::coords> arr = ob.getRadiusCoords_cannon(j);
			cout << "\n to be upgraded are==>" << arr.size();
			for (int i = 0; i < arr.size(); i++)
			{
				cout << arr[i].r << " " << arr[i].c;
			}
			for (int i = 0; i < arr.size(); i++)
			{
				ob.Greed_updateCost(arr[i], 50);
			}
		}
		
		
		
		int frame_rate = 0;
		
		for (int i = 1; i <= 25; i++)
		{
			cout << ob.Greed_sail(Direction::EAST) << " ";
		}
		ob.threshold_health = 20;
		while (1)
		{
			
			if (ob.frame_rate_limiter())
			{    //this is anchit rana talking to the world and i want ot know the difference between
				//cout<<"\n my health is==>"<<ob.getCurrentHealth();
				
				

			}

		}
			


	}


