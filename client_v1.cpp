#pragma once
#include<SFML/Graphics.hpp>


#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() WSAGetLastError()


#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#include "lib2.hpp"
#include "lib2.cpp"
/*
* what am i doing here?
* the entire template is in lib2.hpp and its cpp counterpart
* so everytime we need to make a new version of the pre-existing game, just import the Game API with all implementations
* and write your own graphics::callable function and a handler function void main() to complete it
* this way we can create any variant of the game using only the pre-built template.
* PS: any extra classes required can be written here
*/
/*
* about this client:
* this is client_v1 which will run the algorithm of the user and send and recv the data from the server about the other servers and the game states
* sending part:
* send all the decisions taken by the algorithm, position and the firing
* recv part:
* recv the information of other ships enough to process its own algorithm
*/

class Startup_info
{
	/*
	* configurations server will send to the client for the initialization of the game
	*/
	int framerateLimit;
	int no_of_players;
	double game_time;//time for which the game will run
	
	friend void graphics::callable(Mutex*, int code[rows][columns], Map&);
};
class GameState
{
	/* this class has the format of data that the server will send to the client*/
	bool collision;
	int size_collide_ship;//size of the array below
	int collide_ship[10];//id's of the ships that collided
	friend class graphics;

};
class shipData_forServer
{
	/* this class has the format of data that the client will send to the server
	*/
	
	int ship_id;//id of the ship

	bool isFiring;
	double threshold_health;
	double threshold_ammo;
	double threshold_fuel;
	int seconds;//seconds lived

	int minutes;//minutes lived
	Greed::coords tile_pos_front;
	Greed::coords tile_pos_rear;
	Greed::abs_pos front_abs_pos;//topmost coordinates of the tip of the ship:: will be updated in update_tile_pos
	Greed::abs_pos rear_abs_pos;//endmost coordinates of the ship ==> will be updated in update_tile_pos
	//Map::abs_pos absolutePosition;//always stores the top left coordinate of the ship tile
	Direction dir;
	int radius;//square radius
	int bullet_radius;//radius of the bullet...initially this value is equal to the radius of the ship
	
	Greed::abs_pos absolutePosition;
	int autopilot = 0;//bit to check if the ship is moving in autopilot or not
	friend class graphics;
};

void graphics::callable(Mutex* mutx, int code[rows][columns], Map& map_ob)
{
	
	Control nm;
	Startup_info info = nm.getStartupInfo();//get the initial configuration of the game
	deque<ship*>& pl1 = nm.getShipList(2369);
	List<Greed::cannon>& cannon_list = nm.getCannonList(2369);
	//leaving the opaque coordinates list, if needed later it will be imported
	sf::Clock clock;
	double elapsed_time = 0;
	int c = 0;
	double t2 = 0;
	int frames = 0;
	bool gameOver = false;
	//starting the game loop that will sync the algorithm execution with the gameplay
	int cur_frame = -1;//variable to maintain the frame rate of the while loop
	int next_frame = -1;

	//THIS code is for updating the fuel of the ship
	Greed::coords* prev = new Greed::coords[pl1.size()];
	for (int i = 0; i < pl1.size(); i++)//setting the previous position
	{
		prev[i] = pl1[i]->tile_pos_front;
	}
	GameState data;

	while (1)
	{
		sf::Time curtime = clock.restart();
		elapsed_time += curtime.asSeconds();
		next_frame = elapsed_time * info.framerateLimit;
		if (elapsed_time > 1)
		{
			elapsed_time = 0;
		}
		if (next_frame != cur_frame)
		{
			cur_frame = next_frame;
			//for handling the ship

			if (!gameOver)
			{
				for (int i = 0; i < pl1.size(); i++)
				{
					//getPointPath has to be protected by a mutex
					if (pl1[i]->died == 1)
					{
						continue;
					}

					if (mutx->m[pl1[i]->getMutexId(2369)].try_lock())
					{
						if (data.collision)//if the server says that it is a collision
						{
							//collision is settled.
							for (int i = 0; i < data.size_collide_ship; i++)
							{
								pl1[i]->collided_ships.clear();
								pl1[i]->collided_ships.push_back(data.collide_ship[i]);

								mutx->m[i].unlock();
								pl1[i]->anchorShip_collision();
								mutx->m[i].lock();
							}
						}
						List<Greed::abs_pos>& l1 = pl1[i]->getPathList(2369);

						if (pl1[i]->fuel > 0 && l1.howMany() > 0 && l1.howMany() - 1 != pl1[i]->getPointPath(2369))
						{
							// cout << "\n fuel " << i << " now=>" << pl1[i]->fuel;
							pl1[i]->motion = 1;

							pl1[i]->update_pointPath(pl1[i]->getPointPath(2369) + 1, 2369);

							pl1[i]->rect.setPosition(::cx(l1[pl1[i]->getPointPath(2369)].x + origin_x), ::cy(l1[pl1[i]->getPointPath(2369)].y + origin_y));

							pl1[i]->update_tile_pos(l1[pl1[i]->getPointPath(2369)].x, l1[pl1[i]->getPointPath(2369)].y);

							//update the fuel here for this ship
							
						}
						else
						{
							pl1[i]->motion = 0;
						}
						if (pl1[i]->ammo > 0 && pl1[i]->cannon_ob.activeBullets.size() > 0 && frames % 30 == 0)
						{
							frames = 0;
						
								Greed::bullet bull = pl1[i]->cannon_ob.activeBullets[0];
								auto it = pl1[i]->cannon_ob.activeBullets.begin();
								pl1[i]->cannon_ob.activeBullets.erase(it);
								if (bull.target_ship != -1)
									pl1[i]->setBullet(bull, bull.can, bull.target_ship, bull.s);
								else if (bull.target_cannon != -1)
								{
									pl1[i]->setBullet_forCannon(bull);
								}
								pl1[i]->cannon_ob.legal_bullets[bull.id] = bull;
							pl1[i]->isFiring = true;


						}
						else
						{
							pl1[i]->isFiring = false;
						}
						//firing managing starts here
						for(int i=0;i<pl1[i]->cannon_ob.legal_bullets.size();i++)
						{
							auto it = pl1[i]->cannon_ob.legal_bullets.begin();
							advance(it, i);

							if (it->second.bullet_trajectory.size() > 0)
							{
								it->second.absolute_position = Greed::abs_pos(it->second.bullet_trajectory[0].x, it->second.bullet_trajectory[0].y);
								it->second.bullet_entity.setPosition(sf::Vector2f(::cx(it->second.bullet_trajectory[0].x + origin_x), ::cy(it->second.bullet_trajectory[0].y + origin_y)));
								for (int l = 1; l <= 3; l++)//this is speed of bullet. skipping 3 pixels per frame, speed is 180 frames(pixels) per sec
								{
									if (it->second.bullet_trajectory.size() > 0)
									{
										it->second.bullet_trajectory.erase(it->second.bullet_trajectory.begin());
									}
								}
							}
							else //when bullet's trajectory is over and nothing happend
							{
								pl1[i]->cannon_ob.legal_bullets.erase(it);
								i--;
								continue;
							}
							
							if (data.hitOpaque)//if bullet hit an opaque object delete it!
							{
								pl1[i]->cannon_ob.legal_bullets.erase(it);
								i--;
								continue;
										
							}
							if (data.collison_bullet)//collision with ship
							{
								it->second.set_after_data(5, pl1[u]->ship_id, true);//the bullet is still active for animation
								it->second.hit_or_not = true;
								// cout << "\n collision taken";
								pl1[i]->cannon_ob.legal_bullets.erase(it);
								i--;
								continue;
							}

							//checking if the bullet hit a cannon

							if (cannon_list[k].tile == cor)
							{
												// pl1[i]->cannon_ob.allBullets[it->second.id].set_after_data_for_cannon(5, cannon_list[k].cannon_id, true);
								it->second.set_after_data_for_cannon(5, cannon_list[k].cannon_id, true);
								pl1[i]->cannon_ob.legal_bullets.erase(it);
								i--;
							}
									
								
								// cout << "\n end";

						}
						/* ends */

					}
				}
			}
		}
			
	}

}




