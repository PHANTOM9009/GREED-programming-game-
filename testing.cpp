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
	ob.Greed_chaseShip(0);
	//ob.Greed_setPath(Greed::coords(3,9));
	while (1)
	{
		elapsed_time += clock.restart().asSeconds();
		if (elapsed_time > 1)
		{
			//cout << "\n the frame rate of the algorithm is==>" << frame_rate;
			elapsed_time = 0;
			frame_rate = 0;

		}
		if (ob.frame_rate_limiter())

		{//this is anchit rana talking to the world and i want ot know the difference between
			
			frame_rate++;
			cout << "\n motion is==>" << ob.isShipInMotion();
			
			Event e;
			ob.getNextCurrentEvent(e);
			if (e.eventType == Event::EventType::ShipsInMyRadius)
			{
				cout << "\n active event ship is in my radius==>" << e.radiusShip.getShipId()[0];
			}
			if (e.eventType == Event::EventType::CannonsInMyRadius)
			{
				
			}
			if (e.eventType == Event::EventType::ShipCollision)
			{
				cout << "\n collided with==>" << e.shipCollision.getShipId()[0];
				ob.Greed_setPath(Greed::coords(3, 9));
			}
			if (!ob.isShipInMotion())
			{
				ob.Greed_chaseShip(0);
				cout << "\n chasing the sh ip===?";
			}
			if (ob.getCurrentFuel() <= 5)
			{
				ob.Greed_upgradeFuel(10);
			}
			deque<Event> passive = ob.getPassiveEvent();
			for (int i = 0; i < passive.size(); i++)
			{
				if (passive[i].eventType == Event::EventType::ShipsInMyRadius)
				{
					//cout << "\n passive event ship is in my radius==>" << passive[i].radiusShip.getShipId()[0];
						
				}
			}
			/*
			for (int j = 0; j < ob.cannonsInMyRadius().size(); j++)
			{
				//cout << "\n cannon in my radius is==>" << ob.cannonsInMyRadius()[j];
				cout << "\n calling anchorship=====================================?>";
				ob.Greed_anchorShip();
			}
			if (!ob.isShipInMotion())
			{
				ob.Greed_chaseShip(0);
			}
			*/	
			if (ob.getCurrentHealth() <= 10)
			{
				ob.Greed_upgradeHealth(10);
			}
			
		}

	}



}

