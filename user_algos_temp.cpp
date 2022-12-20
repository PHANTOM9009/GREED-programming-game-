#include<iostream>


void user1(ship& ob)
{

	ob.chaseShip(1);
	
	ob.threshold_health = 50;
	vector<Greed::coords> l = ob.getRadiusCoords_ship(0);
	
	while (1)
	{
		
		Sleep(5);
		//Sleep(500);
	   //if(ob.current_event.size()>0)
	   //cout<<"\n event for ship 0==>"<<(int)ob.getNextEvent().eventType;
	   //ob.fireCannon(cannon::FRONT, 1, ShipSide::FRONT);

		Event e;
		ob.getNextCurrentEvent(e);
		if (e.eventType == Event::EventType::LowHealth)
		{
			//cout << "\n health is==>" << e.lowHealth.getHealth();
			ob.upgradeHealth(20);
		}
		deque<Event> q = ob.getPassiveEvent();
		for (int i = 0; i < q.size(); i++)
		{
			if (q[i].eventType == Event::EventType::ShipsInMyRadius)
			{
				//if (frames % 60 == 0)
				//{
				
				ob.fireCannon(cannon::FRONT, q[i].radiusShip.getShipId()[0], ShipSide::FRONT);
				//}

			}
			if (q[i].eventType == Event::EventType::CannonsInMyRadius)
			{
				ob.fireAtCannon(q[i].radiusCannon.getCannonId()[0], cannon::FRONT);
			}
			
		}
		if (ob.getCurrentAmmo() < 5)
		{
			ob.upgradeAmmo(10);
		}
		if (ob.getShipList()[1].getDiedStatus() == 1)
		{

			ob.ceaseFire();
		}

	}


}

vector<int> no;
void user2(ship& ob)//2nd player function
{

	//ob.sail(Direction::NORTH, 10);
	Greed::path_attribute path = ob.setTarget(Greed::coords(0, 2));
	ob.setPath(path.target);
	int count = 0;
	//ob.chaseShip(0);
	ob.fireCannon(cannon::FRONT, 0, ShipSide::FRONT);
	
	double elapsed_time = 0;
	int c = 0;
	int frames = 0;

	while (1)
	{
		
		Event e;
		ob.getNextCurrentEvent(e);
		if (ob.getCurrentHealth() <= 10)
		{
			ob.upgradeHealth(10);
		}
		deque<Event> q = ob.getPassiveEvent();
		for (int i = 0; i < q.size(); i++)
		{
			if (q[i].eventType == Event::EventType::ShipsInMyRadius)
			{
				//if (frames % 60 == 0)
				//{
				frames = 0;
				ob.fireCannon(cannon::FRONT, q[i].radiusShip.getShipId()[0], ShipSide::FRONT);
				//}

			}
		}
		if (ob.getCurrentAmmo() < 1)
		{
			ob.upgradeAmmo(10);
		}
		if (ob.getShipList()[0].getDiedStatus() == 1)
		{

			ob.ceaseFire();
		}

		/*
		Event e;
		while (ob.getNextCurrentEvent(e))
		{
			if (e.eventType == Event::EventType::ShipFire)
			{

					ob.fireCannon(cannon::REAR, 0, ShipSide::FRONT);


			}
		}
		*/

	}

	/*
	while (1)
	{


		no_of_frames++;

		deltime = clock.restart();
		elapsed_time += deltime.asSeconds();
		if ((no_of_frames) % (90000) == 0)
		{
			c++;
			if (elapsed_time >= 1 && elapsed_time <= 1.15)
			{
				cout << "\n no of frames are==>" <<c << endl;
				no.push_back(c);
				no_of_frames = 0;
				c = 0;
				elapsed_time = 0;
			}


			//Sleep(25);
			//ob.fireAtCannon(0, cannon::FRONT);
			//ob.fireCannon(cannon::FRONT, 0, ShipSide::FRONT);
			//Sleep(500);

			Event e;
			while (ob.getNextCurrentEvent(e))
			{
				if (e.eventType == Event::EventType::ShipFire)
				{
					ob.fireCannon(cannon::FRONT, 0, ShipSide::FRONT);
				}
			}

		}

	}
	*/


}

