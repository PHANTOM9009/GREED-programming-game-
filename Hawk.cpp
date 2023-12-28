
#include "online_lib2.hpp"


	int find_ship_to_kill(deque<shipInfo>& shipList, int myid, ship& ob,int hate_id)
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
	Greed::coords runAway(vector<int> ship_id,ship &ob)
	{
		Greed::coords address(0,0);
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
						if (!ob.isInShipRadius(ship_id[k],Greed::coords(i,j)))
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
			for (int i = 0; i < arr.size(); i++)
			{
				ob.Greed_updateCost(arr[i], 50);
			}
		}
		
		deque<shipInfo> shipList = ob.getShipList();
		int index = find_ship_to_kill(shipList, ob.getShipId(), ob, ob.getShipId());
		//ob.Greed_chaseShip(index);
		int frame_rate = 0;
		
		double elapsed_time = 0;
		while (1)
		{

			if (ob.frame_rate_limiter())
			{//this is anchit rana talking to the world and i want ot know the difference between
				//cout<<"\n my health is==>"<<ob.getCurrentHealth();
				frame_rate++;
				
				if (elapsed_time > 1)
				{
				
					frame_rate = 0;
					elapsed_time = 0;
				}
			//	cout << "\n my health  is=>" << ob.getCurrentHealth();
				//ob.chaseShip(2);
				deque<shipInfo> shipList = ob.getShipList();
				Event e;
				//cout << "\n position of alternate ship==>" << shipList[0].getCurrentTile().r << " " << shipList[0].getCurrentTile().c;
				//cout << "\n position of my ship==>" <<ob.getCurrentTile().r<<" "<<ob.getCurrentTile().c;//anchit rana is the greatest man in the whole wold and we all know that cheers
				ob.getNextCurrentEvent(e);
												
				if (e.eventType == Event::EventType::ShipFire)
				{
					for (auto it : e.shipFire.getShipId())
					{
						ob.Greed_fireCannon(cannon::FRONT, it.first, ShipSide::FRONT);
					}
				}
				//ob.Greed_fireCannon(cannon::FRONT, 0, ShipSide::REAR);
				if (e.eventType == Event::EventType::LowAmmo)
				{
					ob.Greed_upgradeAmmo(20);
				}
				if (e.eventType == Event::EventType::LowHealth)
				{
					ob.Greed_upgradeHealth(5);
				}
			
				if (e.eventType == Event::EventType::ShipsInMyRadius)
				{
					
					
				}
				
				deque<Event> q = ob.getPassiveEvent();

				for (int i = 0; i < q.size(); i++)
				{
					
					if (q[i].eventType == Event::EventType::CannonsInMyRadius)
					{
						
						vector<Greed::cannon> ls = ob.getCannonList();
						if (ls[q[i].radiusCannon.getCannonId()[0]].isCannonDead() == 0)
						{
							ob.Greed_fireAtCannon(q[i].radiusCannon.getCannonId()[0], cannon::FRONT);
						}
					}
					if (q[i].eventType == Event::EventType::ShipsInMyRadius)
					{
						
					}

				}
							
				


			}

		}
			


	}


