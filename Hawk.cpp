#include "online_lib2.hpp"
namespace user1
{

	int find_ship_to_kill(deque<shipInfo>& shipList, int myid, ship& ob,int hate_id)
	{
		int mini = INT_MAX;
		int index = 0;
		for (int i = 0; i < shipList.size(); i++)
		{
			if (shipList[i].getDiedStatus() == 0 && i != myid && i!=hate_id)
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
		deque<shipInfo> shipList = ob.getShipList();
		int index = find_ship_to_kill(shipList, ob.getShipId(), ob, ob.getShipId());
		if (index >= 0)
		ob.Greed_chaseShip(index);
		
		while (1)
		{

			if (ob.frame_rate_limiter())
			{

				//ob.chaseShip(2);
				deque<shipInfo> shipList = ob.getShipList();
				Event e;
				
				ob.getNextCurrentEvent(e);
				if (index >= 0 && shipList[index].getDiedStatus() == 1)
				{
					index = find_ship_to_kill(shipList, ob.getShipId(), ob,ob.getShipId());
					ob.Greed_chaseShip(index);
					//cout << "\n called find_ship_to_kill";
					//cout << "\n my id=>" << ob.getShipId() << " target id=>" << index;
				}
			
				
				if (e.eventType == Event::EventType::LowHealth)
				{
					ob.Greed_upgradeHealth(20);

					std::time_t result = std::time(nullptr);
					//cout << "\n time=>" << std::localtime(&result)->tm_hour << ":" << std::localtime(&result)->tm_min << ":" << std::localtime(&result)->tm_sec;
				}
				if (e.eventType == Event::EventType::ShipFire)
				{
					
					for (auto it : e.shipFire.getShipId())
					{
						ob.Greed_fireCannon(cannon::FRONT, it.first, ShipSide::REAR);
					}
				}
				ob.Greed_fireCannon(cannon::FRONT, 0, ShipSide::REAR);
				if (e.eventType == Event::EventType::LowAmmo)
				{
					//ob.Greed_upgradeAmmo(20);
				}
				if (e.eventType == Event::EventType::ShipCollision)
				{
					cout << "\n my ship collided,"<<e.shipCollision.getShipId()[0]<<" finding new hunt..";
					index = find_ship_to_kill(shipList, ob.getShipId(), ob,e.shipCollision.getShipId()[0]);
					cout << "\n new hunt is==>" << index;
					ob.Greed_chaseShip(index);

				}
				deque<Event> q = ob.getPassiveEvent();

				for (int i = 0; i < q.size(); i++)
				{
					if (q[i].eventType == Event::EventType::ShipsInMyRadius)
					{
						
						for (int j = 0; j < q[i].radiusShip.getShipId().size(); j++)
						{
							
							if (q[i].radiusShip.getShipId()[j] == -1)
							{
								continue;
							}
							ob.Greed_fireCannon(cannon::FRONT, q[i].radiusShip.getShipId()[j], ShipSide::FRONT);
						}
					}
					if (q[i].eventType == Event::EventType::CannonsInMyRadius)
					{
						vector<Greed::cannon> ls = ob.getCannonList();
						if (ls[q[i].radiusCannon.getCannonId()[0]].isCannonDead() == 0)
						{
							ob.Greed_fireAtCannon(q[i].radiusCannon.getCannonId()[0], cannon::FRONT);
						}
					}

				}

				
				if (ob.getCurrentAmmo() < 10)
				{
					
					ob.Greed_upgradeAmmo(10);
				}
				


			}

		}
			


	}
}

