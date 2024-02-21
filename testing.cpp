
#include "online_lib2.hpp"


int find_ship_to_kill(deque<shipInfo>& shipList, int myid, ship& ob, int hate_id)
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
Greed::coords runAway(vector<int> ship_id, ship& ob)
{
	Greed::coords address(0, 0);
	int distance = 0;
	deque<shipInfo> shipList = ob.getShipList();
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 24; j++)
		{
			if (ob.whatsHere(Greed::coords(i, j)).getEntity() == Entity::WATER)
			{
				for (int k = 0; k < ship_id.size(); k++)
				{
					if (!ob.isInShipRadius(ship_id[k], Greed::coords(i, j)))
					{
						int temp = abs(i - shipList[ship_id[k]].getCurrentTile().r) + abs(j - shipList[ship_id[k]].getCurrentTile().c);
						if (distance < temp)
						{
							distance = temp;
							address = Greed::coords(i, j);
						}
					}
				}
			}
		}
	}
	return address;
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

	double elapsed_time = 0;
	sf::Clock clock;
	ob.threshold_health = 20;
	while (1)
	{
		elapsed_time += clock.restart().asSeconds();
		if (ob.frame_rate_limiter())
		{//this is anchit rana talking to the world and i want ot know the difference between
			//cout<<"\n my health is==>"<<ob.getCurrentHealth();
			if (ob.isShipInMotion() == 0)
			{
				cout << "\n ship motion is off...............";
			}
			Event e;
			ob.getNextCurrentEvent(e);
			if (e.eventType == Event::EventType::ShipFire)
			{
				for (auto it : e.shipFire.getShipId())
				{
					ob.Greed_fireCannon(cannon::FRONT, it.first, ShipSide::FRONT);
				}
			}
			if (e.eventType == Event::EventType::ShipsInMyRadius)
			{
				for (int i = 0; i < e.radiusShip.getShipId().size(); i++)
				{
					ob.Greed_fireCannon(cannon::FRONT, e.radiusShip.getShipId()[i], ShipSide::FRONT);
				}
			}
			if (ob.getCurrentHealth() <= 10)
			{
				ob.Greed_upgradeHealth(10);
			}
			if (ob.getCurrentAmmo() <= 10)
			{
				ob.Greed_upgradeAmmo(10);
			}



		}

	}



}


