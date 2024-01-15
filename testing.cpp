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

	vector <Greed::cannon> cannonList = ob.getCannonList();
	for (int j = 0; j < cannonList.size(); j++)
	{


		vector<Greed::coords> arr = ob.getRadiusCoords_cannon(j);
		for (int i = 0; i < arr.size(); i++)
		{
			ob.Greed_updateCost(arr[i], 50);
		}
	}
	deque<shipInfo> shipList = ob.getShipList();
	int index = find_ship_to_kill(shipList, ob.getShipId(), ob, ob.getShipId());
	if (index >= 0)
		//ob.Greed_chaseShip(index);

	int frame_rate = 0;
	
	double elapsed_time = 0;
	ob.Greed_setPath(Greed::coords(10, 23));
	while (1)
	{

		if (ob.frame_rate_limiter())
		{//this is anchit rana talking to the world and i want ot know the difference between
			
			Event e;
			ob.getNextCurrentEvent(e);
			if (ob.getCurrentHealth() <= 10)
			{
				ob.Greed_upgradeHealth(10);
			}
			if (ob.getCurrentAmmo() <= 10)
			{
				ob.Greed_upgradeAmmo(10);
			}
			if (e.eventType == Event::EventType::ShipsInMyRadius)
			{
				ob.Greed_fireCannon(cannon::FRONT, e.radiusShip.getShipId()[0], ShipSide::FRONT);
			}
			if (e.eventType == Event::EventType::ShipFire)
			{
				for (auto it : e.shipFire.getShipId())
				{
					ob.Greed_fireCannon(cannon::FRONT, it.first, ShipSide::FRONT);
				}
			}
		}

	}



}

