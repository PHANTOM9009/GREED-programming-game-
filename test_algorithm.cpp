
namespace user1
{



	void GreedMain(ship& ob)
	{
		Greed::path_attribute path = ob.setTarget(Greed::coords(1,1));
		ob.setPath(path.getPath());

		while (1)
		{

			if (ob.frame_rate_limiter())
			{

				//ob.chaseShip(2);
				Event e;
				if (ob.getCurrentHealth() < 50)
				{
					ob.upgradeHealth(20);
				}
				deque<Event> q = ob.getPassiveEvent();

				for (int i = 0; i < q.size(); i++)
				{
					if (q[i].eventType == Event::EventType::ShipsInMyRadius)
					{
						for (int j = 0; j < q[i].radiusShip.getShipId().size(); j++)
						{
							ob.fireCannon(cannon::FRONT, q[i].radiusShip.getShipId()[j], ShipSide::FRONT);
						}
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


			}

		}


	}
}

