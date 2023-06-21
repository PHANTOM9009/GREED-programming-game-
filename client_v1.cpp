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

#include "online_lib2.hpp"
#include "online_lib2.cpp"
#include "hawk.cpp"
#include<ctime>
/*
* what am i doing here?
* the entire template is in lib2.hpp and its cpp counterpart
* so everytime we need to make a new version of the pre-existing game, just import the Game API with all implementations
* and write your own graphics::callable function and a handler function void main() to complete it
* this way we can create any variant of the game using only the pre-built template.
* PS: any extra classes required can be written here
* the lib2 versions are different from offline one's so dont fucking merge them
*/
/*
* about this client:
* this is client_v1 which will run the algorithm of the user and send and recv the data from the server about the other servers and the game states
* sending part:
* send all the decisions taken by the algorithm, position and the firing
* recv part:
* recv the information of other ships enough to process its own algorithm
*/



bool find(int id, int hit[100],int size)
{
	for (int i = 0; i < size; i++)
	{
		if (id == hit[i])
		{
			return true;
		}
	}
	return false;
}

SOCKET connect_to_server()//first connection to the server
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		cout << "\n failed to initialize";
	}
#endif // defined
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	int res = 0;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* server_add;
	char buff[1000];
	getaddrinfo("192.168.129.213", "8080", &hints, &server_add);
	getnameinfo(server_add->ai_addr, server_add->ai_addrlen, buff, sizeof(buff), 0, 0, NI_NUMERICHOST);
	cout << "\n the server address is==>" << buff;
	cout << endl;
	SOCKET peer_socket = socket(server_add->ai_family, server_add->ai_socktype, server_add->ai_protocol);
	cout << "\n socket number of the user is==>" << peer_socket;
	if (!ISVALIDSOCKET(peer_socket))
	{
		cout << "\n socket not created==>" << GETSOCKETERRNO();
	}
	/**/
	while(connect(peer_socket, server_add->ai_addr, server_add->ai_addrlen))
	{
		cout << "\n problem in connecting";

	}
	cout << "\n client connected with server";
	int yes = 1;
	if (setsockopt(peer_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes)) < 0) //disabling nagle's algorithm for speed in sending the data
	{
		fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO());
	}
	//cout << "\n connected with the server..";
	freeaddrinfo(server_add);
	
	int r = recv(peer_socket, (char*)&buff, sizeof(buff), 0);
	cout << "\n the message is=>" << buff;
	return peer_socket;
}

void graphics::callable_client(int ship_id,Mutex* mutx, int code[rows][columns], Map& map_ob,int peer_s,ship &player)
{

	SOCKET peer_socket = peer_s;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	Control nm;
	
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
	control1 control_ob;
	//THIS code is for updating the fuel of the ship

	GameState data;
	long total_time = 0;
	long double frame_number = -1;//this is for packet id that the client will send to the server

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	fd_set master;
	FD_ZERO(&master);
	FD_SET(peer_socket, &master);
	fd_set reads;
	fd_set writes;
	FD_ZERO(&reads);
	FD_ZERO(&writes);

	vector<Greed::bullet> newBullets;
	
	thread t(user1::GreedMain, ref(player));
	t.detach();
	int frame_no = 0;
	double avg_nav_req = 0;
	int total_frames = 0;
	int tot = 0;
	int frame_rate = 0;
	double check_time = 0;
	sf::Clock clock1;
	int c = 0;
	double avg_recv = 0;
	double avg_send = 0;
	double avg_processing = 0;
	while (1)
	{
		/*NOTES:
		* basic architecture of the game loop for client_v1 is:
		* recv the data from the server( this data will have the game state for that frame and information about other players)
		* process/prepare the data (prepare your own state)
		* send the data (data of your ship) to the server
		* 
		* select will return irrespective of the fact that there is any data to read or not.
		* if the connection is interrupted...between the server and client. the client will go on calculating its own state from a static world(since there is no incoming data)
		* but the disconnected client will not move in the game(since there is no connection and for server this client is static)
		* if the connection resumes. the client will not start from where it left, but it will keep on executing instructions since data is not transmitted
		* it will look like magic. the cleint may be dead by the time it gets the connection back.
		* the states will not be consistent in such a case
		* 
		*/
		sf::Time curtime = clock.restart();
		elapsed_time += curtime.asSeconds();
		if (elapsed_time > 1)
		{
			elapsed_time = 0;
		}
		next_frame = elapsed_time * 60;
	
		if (next_frame != cur_frame)//under this frame rate is stable
		{
			sf::Time tt = clock1.restart();
			check_time+=tt.asSeconds();
			frame_rate++;
			c++;
		
			if (check_time > 1)
			{
				cout << "\n frame rate is=>" << frame_rate;
				frame_rate = 0;
				check_time = 0;
			}

			reads = master;
			writes = master;
			cur_frame = next_frame;
			//for handling the ship
			total_time++;
			frame_number++;
			frames++;
			frame_no++;//for nav data
			tot++;
			//we will wait for 50 microseconds for the select to return
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 50;
			//select(peer_socket + 1, &reads, 0, 0, &timeout);
			recv_data data1;
			int bytes_recv = -1;
			if (pl1[ship_id]->died == 1)
			{
				break;
			}
			sf::Clock recvt;
			recvt.restart();
			select(peer_socket + 1, &reads, 0, 0, &timeout);
			if (FD_ISSET(peer_socket, &reads))//socket is ready to read from
			{
				memset((void*)&data1, 0, sizeof(data1));
				bytes_recv = recv(peer_socket, (char*)&data1, sizeof(data1), 0);
				
				
				while (bytes_recv < sizeof(data1))
				{
					bytes_recv += recv(peer_socket, (char*)&data1 + bytes_recv, sizeof(data1) - bytes_recv,0);
				}
				if (bytes_recv < 1)//connection is broken
				{
					//try to reconnect to the server
					cout << "\n average bullets are==>" << avg_bullet / no_of_times;
					cout << "\n jab aaye toh itne aaye=>" << avg_nav_req / total_frames;
					cout << "\n avg_nav_req over total frames=>" << avg_nav_req / tot;
					cout << "\n connection disconnected to the server..retrying to connect";
					//peer_socket = connect_to_server();
					cout << "\n connection is back.";
					break;
				}
				//we have received the data.. now parse the data in original class structure.
				if (bytes_recv > 0)
				{
					//cout << "\n for frame=>" << total_time;
					/*
					cout << "\n the data received by the server is==>";
					//print all the members of data1
					cout << "\n the frame number is=>" << data1.packet_id;
					cout << "\n the ship id is=>" << data1.s1;
					cout << "\n ship_data for me=>";
					cout << "\n ship id=>" << data1.shipdata_forMe.ship_id;
					cout << "\n seconds=>" << data1.shipdata_forMe.seconds;
					cout << "\n minutes=>" << data1.shipdata_forMe.minutes;
					cout << "\n health=>" << data1.shipdata_forMe.health;
					cout << "\n gold=>" << data1.shipdata_forMe.gold;
					cout << "\n died=>" << data1.shipdata_forMe.died;
					cout << "\n ammo=>" << data1.shipdata_forMe.ammo;
					cout << "\n fuel=>" << data1.shipdata_forMe.fuel;
					cout << "\n front tile=>" << data1.shipdata_forMe.front_tile.r << " " << data1.shipdata_forMe.front_tile.c;
					cout << "\n position is=>" << data1.shipdata_forMe.absolute_position.x << " " << data1.shipdata_forMe.absolute_position.y;
					*/
					control_ob.packet_to_pl(data1.shipdata_exceptMe, data1.s1, ship_id, pl1);
					control_ob.packet_to_me(data1.shipdata_forMe, ship_id, pl1);
				}
				

			}
			avg_recv += recvt.getElapsedTime().asSeconds();
			
			//std::time_t result = std::time(nullptr);
		    //cout << "\n----------------------------------------------------------";
			//cout << "\n time=>"<<std::localtime(&result)->tm_hour<<":"<< std::localtime(&result)->tm_min<<":"<< std::localtime(&result)->tm_sec << " client frame = >" << total_time << " " << " received frame = >" << data1.packet_id;
			
			if (!gameOver)
			{
				//using nav_data
				sf::Clock process;
				process.restart();
				if (pl1[ship_id]->nav_data.size() > 0 && frame_no % 120 == 0)//once every two frame
				{
					total_frames++;
					avg_nav_req += pl1[ship_id]->nav_data.size();
					frame_no = 0;
					pl1[ship_id]->nav_data_final.push_back(pl1[ship_id]->nav_data[0]);
					pl1[ship_id]->nav_data.clear();
				}
				
				//getPointPath has to be protected by a mutex
								
				//here we are starting to manage the events
				//checking and deleting events from passive_event queue
			    // /*
			    // here we will delete all the events which are not happening
			    //checking for event1
				
					if (mutx->event_mutex[ship_id].try_lock())
					{
						//deleting from the current_event

						pl1[ship_id]->current_event_point = 0;
						for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
						{
							//checking for event1
							if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::ShipsInMyRadius)
							{
								//checking if this event is still happening, if not then delete this event
								bool check = false;
								vector<int> l = pl1[ship_id]->shipsInMyRadius();
								if (l.size() == pl1[ship_id]->passive_event[j].radiusShip.sid.size())
								{
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
									{
										if (l[k] != pl1[ship_id]->passive_event[j].radiusShip.sid[k])
										{
											check = true;
											break;

										}
									}
									if (check == true)//event has to be deleted
									{
										auto it = pl1[ship_id]->passive_event.begin();
										advance(it, j);
										pl1[ship_id]->passive_event.erase(it);
										j--;
									}
								}
								else
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}

							}
							//checking for next event
							//event2
							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInShipRadius)
							{
								bool check = false;
								vector<int> l = pl1[ship_id]->shipsIamInRadiusOf();
								if (l.size() == pl1[ship_id]->passive_event[j].radiusShip.sid.size())
								{
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
									{
										if (l[k] != pl1[ship_id]->passive_event[j].radiusShip.sid[k])
										{
											check = true;
											break;

										}
									}
									if (check == true)//event has to be deleted
									{
										auto it = pl1[ship_id]->passive_event.begin();
										advance(it, j);
										pl1[ship_id]->passive_event.erase(it);
										j--;
									}
								}
								else
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}

							}
							//checking for event3
							else	if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInCannonRadius)
							{
								bool check = false;
								vector<int> l = pl1[ship_id]->cannonsIamInRadiusOf();
								if (l.size() == pl1[ship_id]->passive_event[j].radiusCannon.cid.size())
								{


									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
									{

										if (l[k] != pl1[ship_id]->passive_event[j].radiusCannon.cid[k])

										{
											check = true;
											break;

										}
									}
									if (check == true)//event has to be deleted
									{
										auto it = pl1[ship_id]->passive_event.begin();
										advance(it, j);
										pl1[ship_id]->passive_event.erase(it);
										j--;
									}
								}
								else
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							//checking for event4
							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::CannonsInMyRadius)
							{
								bool check = false;
								vector<int> l = pl1[ship_id]->cannonsInMyRadius();
								if (l.size() == pl1[ship_id]->passive_event[j].radiusCannon.cid.size())
								{
									for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
									{
										if (l[k] != pl1[ship_id]->passive_event[j].radiusCannon.cid[k])
										{
											check = true;
											break;

										}
									}
									if (check == true)//event has to be deleted
									{
										auto it = pl1[ship_id]->passive_event.begin();
										advance(it, j);
										pl1[ship_id]->passive_event.erase(it);
										j--;
									}
								}
								else
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							//checking for event7
							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowHealth)
							{

								if (pl1[ship_id]->getCurrentHealth() > pl1[ship_id]->threshold_health)
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							//checking for event8 
							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoAmmo)
							{
								if (pl1[ship_id]->getCurrentAmmo() > 0)
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							//checking for event9
							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowAmmo)
							{
								if (pl1[ship_id]->getCurrentAmmo() > pl1[ship_id]->threshold_ammo)

								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							//checking for event10

							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoFuel)
							{
								if (pl1[ship_id]->getCurrentFuel() > 0)
								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}
							//checking for event 11
							else if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowFuel)
							{
								if (pl1[ship_id]->getCurrentFuel() > pl1[ship_id]->threshold_fuel)

								{
									auto it = pl1[ship_id]->passive_event.begin();
									advance(it, j);
									pl1[ship_id]->passive_event.erase(it);
									j--;
								}
							}

						}
						mutx->event_mutex[ship_id].unlock();

					}
				
				//event handling starts here

			
					//handling events for all the ships
					if (mutx->event_mutex[ship_id].try_lock())
					{
						//important info: here ids are not given to the events, they will be given only after their validation

						//adding events in the queue of the respective ships..
						//checking for shipsInMyRadius event first
						vector<int> l1 = pl1[ship_id]->shipsInMyRadius();
						Event e1;
						//make all the events first then check it with passive_event in a single loop
						if (l1.size() > 0)//it means that event has happened
						{
							e1.initialize(total_time, Event::EventType::ShipsInMyRadius, pl1[ship_id]->ship_id);

							e1.radiusShip = Event::Radius_ship(l1);

							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								//chekcing for event1
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::ShipsInMyRadius && e1.eventType == Event::EventType::ShipsInMyRadius)
								{
									//cout << "\n in event0==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
									if (abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp) <= 5 && pl1[ship_id]->passive_event[j].radiusShip.sid.size() == e1.radiusShip.sid.size())//event occuring in intervals of three frames
									{
										//if these conditions are satisfied then only check equality operation between the events
										gotcha = 1;
										int found = 0;
										for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
										{
											if (!(pl1[ship_id]->passive_event[j].radiusShip.sid[k] == e1.radiusShip.sid[k]))
											{
												found = 1;
												break;
											}
										}
										if (found == 0)//this means that events are same
										{
											pl1[ship_id]->passive_event[j].ttl = e1.timestamp;//updating the last occuring of the event in the passive event queue
											break;
										}
										else if (found == 1)//this is a new event
										{
											e1.setEventId();
											pl1[ship_id]->current_event.push_back(e1);//added the event in the current_event
											pl1[ship_id]->passive_event.push_back(e1);//added the event in the passive_event
											break;
										}
									}


								}
							}
							if (gotcha == 0)
							{

								e1.setEventId();
								pl1[ship_id]->current_event.push_back(e1);//added the event in the current_event
								pl1[ship_id]->passive_event.push_back(e1);//added the event in the passive_event

							}
						}
						//checking for IamInShipRadius

						Event e2;
						vector<int> l2 = pl1[ship_id]->shipsIamInRadiusOf();
						if (l2.size() > 0)
						{
							e2.initialize(total_time, Event::EventType::IamInShipRadius, pl1[ship_id]->ship_id);
							e2.radiusShip = Event::Radius_ship(l2);
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInShipRadius && e2.eventType == Event::EventType::IamInShipRadius)
								{
									//cout << "\n in event1==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
									if (abs(pl1[ship_id]->passive_event[j].ttl - e2.timestamp) <= 5 && pl1[ship_id]->passive_event[j].radiusShip.sid.size() == e2.radiusShip.sid.size())//event occuring in intervals of three frames
									{
										//if these conditions are satisfied then only check equality operation between the events
										gotcha = 1;
										int found = 0;
										for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusShip.sid.size(); k++)
										{
											if (!(pl1[ship_id]->passive_event[j].radiusShip.sid[k] == e2.radiusShip.sid[k]))
											{
												found = 1;
												break;
											}
										}
										if (found == 0)//this means that events are same
										{
											//cout << "\n i worked";
											pl1[ship_id]->passive_event[j].ttl = e2.timestamp;//updating the last occuring of the event in the passive event queue
											break;
										}
										else if (found == 1)//this is a new event
										{
											e2.setEventId();
											pl1[ship_id]->current_event.push_back(e2);//added the event in the current_event
											pl1[ship_id]->passive_event.push_back(e2);//added the event in the passive_event
											break;
										}
									}
									else if (e2.eventType == Event::EventType::IamInShipRadius)
									{
										e2.setEventId();
										pl1[ship_id]->current_event.push_back(e2);//added the event in the current_event
										pl1[ship_id]->passive_event.push_back(e2);//added the event in the passive_event
										break;
									}
								}
							}
							if (gotcha == 0)
							{
								e2.setEventId();
								pl1[ship_id]->current_event.push_back(e2);//added the event in the current_event
								pl1[ship_id]->passive_event.push_back(e2);//added the event in the passive_event
							}

						}
						//making for the event IamInCannonRadius
						Event e3;
						vector<int> l3 = pl1[ship_id]->cannonsIamInRadiusOf();
						if (l3.size() > 0)
						{
							e3.initialize(total_time, Event::EventType::IamInCannonRadius, pl1[ship_id]->ship_id);

							e3.radiusCannon = Event::Radius_cannon(l3);
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::IamInCannonRadius && e3.eventType == Event::EventType::IamInCannonRadius)
								{
									//cout << "\n in event2==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
									if (abs(pl1[ship_id]->passive_event[j].ttl - e3.timestamp) <= 3 && pl1[ship_id]->passive_event[j].radiusCannon.cid.size() == e3.radiusCannon.cid.size())
									{
										gotcha = 1;
										int found = 0;
										for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
										{
											if (!(pl1[ship_id]->passive_event[j].radiusCannon.cid[k] == e3.radiusCannon.cid[k]))
											{
												found = 1;
												break;
											}
										}
										if (found == 1)//the events are not equal this is a new event
										{
											e3.setEventId();
											pl1[ship_id]->current_event.push_back(e3);
											pl1[ship_id]->passive_event.push_back(e3);
											break;

										}
										else if (found == 0)//event is same
										{
											pl1[ship_id]->passive_event[j].ttl = e3.timestamp;
											break;
										}
									}
									else if (e3.eventType == Event::EventType::IamInCannonRadius)
									{
										e3.setEventId();
										pl1[ship_id]->current_event.push_back(e3);
										pl1[ship_id]->passive_event.push_back(e3);
										break;
									}
								}
							}
							if (gotcha == 0)
							{
								e3.setEventId();
								pl1[ship_id]->current_event.push_back(e3);
								pl1[ship_id]->passive_event.push_back(e3);
							}

						}
						//making of the event CannonsInMyRadius
						Event e4;
						vector<int> l4 = pl1[ship_id]->cannonsInMyRadius();
						if (l4.size() > 0)
						{
							e4.initialize(total_time, Event::EventType::CannonsInMyRadius, pl1[ship_id]->ship_id);
							e4.radiusCannon = Event::Radius_cannon(l4);
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::CannonsInMyRadius && e4.eventType == Event::EventType::CannonsInMyRadius)
								{
									//cout << "\n in event3==>" << abs(pl1[ship_id]->passive_event[j].ttl - e1.timestamp);
									if (abs(pl1[ship_id]->passive_event[j].ttl - e4.timestamp) <= 3 && pl1[ship_id]->passive_event[j].radiusCannon.cid.size() == e4.radiusCannon.cid.size())
									{
										int found = 0;
										gotcha = 1;
										for (int k = 0; k < pl1[ship_id]->passive_event[j].radiusCannon.cid.size(); k++)
										{
											if (!(pl1[ship_id]->passive_event[j].radiusCannon.cid[k] == e4.radiusCannon.cid[k]))
											{
												found = 1;
												break;
											}
										}
										if (found == 1)//the events are not equal this is a new event
										{
											e4.setEventId();
											pl1[ship_id]->current_event.push_back(e4);
											pl1[ship_id]->passive_event.push_back(e4);
											break;

										}
										else if (found == 0)//event is same
										{
											pl1[ship_id]->passive_event[j].ttl = e4.timestamp;
											break;
										}
									}
									else if (e4.eventType == Event::EventType::CannonsInMyRadius)
									{
										e4.setEventId();
										pl1[ship_id]->current_event.push_back(e4);
										pl1[ship_id]->passive_event.push_back(e4);
										break;
									}
								}
							}
							if (gotcha == 0)
							{
								e4.setEventId();
								pl1[ship_id]->current_event.push_back(e4);
								pl1[ship_id]->passive_event.push_back(e4);
							}
						}
						//making of the event ShipFire

						Event e5;
						unordered_map<int, Greed::bullet> l5;
						for (int j = 0; j < pl1[ship_id]->bullet_hit_tempo.size(); j++)
						{
							if (pl1[ship_id]->bullet_hit_tempo[j].launch_ship != -1)
							{

								l5.insert(pair<int, Greed::bullet>(pl1[ship_id]->bullet_hit_tempo[j].launch_ship, pl1[ship_id]->bullet_hit_tempo[j]));
								auto it = pl1[ship_id]->bullet_hit_tempo.begin();
								advance(it, j);
								pl1[ship_id]->bullet_hit_tempo.erase(it);
								j--;


							}
						}
						if (l5.size() > 0)//this event is set here
						{
							e5.initialize(total_time, Event::EventType::ShipFire, pl1[ship_id]->ship_id);
							e5.setEventId();
							e5.shipFire = Event::ShipFire(l5);
							pl1[ship_id]->current_event.push_back(e5);
						}

						//making of the event cannonFire
						Event e6;
						unordered_map<int, Greed::bullet> l6;
						for (int j = 0; j < pl1[ship_id]->bullet_hit_tempo.size(); j++)
						{
							if (pl1[ship_id]->bullet_hit_tempo[j].launch_cannon != -1)
							{
								l6.insert(pair<int, Greed::bullet>(pl1[ship_id]->bullet_hit_tempo[j].launch_cannon, pl1[ship_id]->bullet_hit_tempo[j]));
								auto it = pl1[ship_id]->bullet_hit_tempo.begin();
								advance(it, j);
								pl1[ship_id]->bullet_hit_tempo.erase(it);
								j--;

							}

						}
						if (l6.size() > 0)
						{
							e6.initialize(total_time, Event::EventType::CannonFire, pl1[ship_id]->ship_id);
							e6.setEventId();

							e6.cannonFire = Event::CannonFire(l6);
							pl1[ship_id]->current_event.push_back(e6);
							//hey
						}
						//making of the event LowHealth
						Event e7;

						if (pl1[ship_id]->getCurrentHealth() < pl1[ship_id]->threshold_health)//there will be a health threshold that can be set by every player
						{
							e7.initialize(total_time, Event::EventType::LowHealth, pl1[ship_id]->ship_id);
							e7.lowHealth = Event::LowHealth(pl1[ship_id]->getCurrentHealth());
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowHealth && e7.eventType == Event::EventType::LowHealth)
								{
									gotcha = 1;
									if (abs(pl1[ship_id]->passive_event[j].ttl - e7.timestamp) <= 3)//the event need not be repeated
									{
										pl1[ship_id]->passive_event[j].ttl = e7.timestamp;//updating the timestamp
										pl1[ship_id]->passive_event[j].lowHealth.health = e7.lowHealth.health;//updating the new health
									}
									else
									{
										e7.setEventId();
										pl1[ship_id]->passive_event.push_back(e7);
										pl1[ship_id]->current_event.push_back(e7);
										break;


									}
								}
							}
							if (gotcha == 0)
							{
								e7.setEventId();
								pl1[ship_id]->passive_event.push_back(e7);
								pl1[ship_id]->current_event.push_back(e7);
							}
						}
						//making of event NoAmmo
						Event e8;
						if (pl1[ship_id]->getCurrentAmmo() <= 0)
						{
							e8.initialize(total_time, Event::EventType::NoAmmo, pl1[ship_id]->ship_id);
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoAmmo)
								{
									if (abs(pl1[ship_id]->passive_event[j].ttl - e8.timestamp) <= 5)
									{
										pl1[ship_id]->passive_event[j].ttl = e8.timestamp;
										gotcha = 1;
										break;
									}
								}
							}
							if (gotcha == 0)//this is a new event
							{
								e8.setEventId();
								pl1[ship_id]->passive_event.push_back(e8);
								pl1[ship_id]->current_event.push_back(e8);

							}

						}
						//making of the event LowAmmo
						Event e9;
						if (pl1[ship_id]->getCurrentAmmo() < pl1[ship_id]->threshold_ammo)
						{
							e9.initialize(total_time, Event::EventType::LowAmmo, pl1[ship_id]->ship_id);
							e9.lowAmmo = Event::LowAmmo(pl1[ship_id]->getCurrentAmmo());
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowAmmo)

								{
									if (abs(pl1[ship_id]->passive_event[j].ttl - e9.timestamp) <= 5)
									{
										gotcha = 1;
										pl1[ship_id]->passive_event[j].ttl = e9.timestamp;
										break;
									}
								}
							}
							if (gotcha == 0)
							{
								e9.setEventId();
								pl1[ship_id]->passive_event.push_back(e9);
								pl1[ship_id]->current_event.push_back(e9);
							}
						}

						//making of the event NoFuel
						Event e10;
						if (pl1[ship_id]->getCurrentFuel() <= 0)
						{
							e10.initialize(total_time, Event::EventType::NoFuel, pl1[ship_id]->getShipId());
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::NoFuel)

								{
									if (abs(pl1[ship_id]->passive_event[j].ttl - e10.timestamp) <= 5)
									{
										gotcha = 1;
										pl1[ship_id]->passive_event[j].ttl = e10.timestamp;
										break;
									}
								}
							}
							if (gotcha == 0)
							{
								e10.setEventId();
								pl1[ship_id]->passive_event.push_back(e10);
								pl1[ship_id]->current_event.push_back(e10);
							}

						}
						//making of the event LowFuel
						Event e11;
						if (pl1[ship_id]->getCurrentFuel() < pl1[ship_id]->threshold_fuel)
						{
							e11.initialize(total_time, Event::EventType::LowFuel, pl1[ship_id]->getShipId());
							e11.lowFuel = Event::LowFuel(pl1[ship_id]->getCurrentFuel());
							int gotcha = 0;
							for (int j = 0; j < pl1[ship_id]->passive_event.size(); j++)
							{
								if (pl1[ship_id]->passive_event[j].eventType == Event::EventType::LowFuel)

								{
									if (abs(pl1[ship_id]->passive_event[j].ttl - e11.timestamp) <= 5)
									{
										gotcha = 1;
										pl1[ship_id]->passive_event[j].ttl = e11.timestamp;
										break;
									}
								}
							}
							if (gotcha == 0)
							{
								e11.setEventId();
								pl1[ship_id]->passive_event.push_back(e11);
								pl1[ship_id]->current_event.push_back(e11);
							}
						}
						//now checking which ones of them are in passive_event queue and new events will be added in both the queues

						//adding the event of colliding of ships
						Event e12;
						
						if (pl1[ship_id]->collided_ships.size() > 0)//if the event of collision has happend
						{
							//cout << "\n adding to the event";
							e12.initialize(total_time, Event::EventType::ShipCollision, pl1[ship_id]->ship_id);
							e12.setEventId();
							e12.shipCollision = Event::ShipCollision(pl1[ship_id]->collided_ships);
							pl1[ship_id]->current_event.push_back(e12);
							pl1[ship_id]->collided_ships.clear();

						}
						



						mutx->event_mutex[ship_id].unlock();
					}

				
				/*event handling is over here*/

					avg_processing += process.getElapsedTime().asSeconds();
			}
			
			//send the data over here
			sf::Clock sending;
			sending.restart();
			
			if (pl1[ship_id]->died == 0)//send the data only if the ship is alive
			{
				send_data data2;
				shipData_forServer shipdata;
				memset((void*)&shipdata, 0, sizeof(shipdata));
				memset((void*)&data2, 0, sizeof(data2));
				
				select(peer_socket + 1, 0, &writes, 0, &timeout);

				if (FD_ISSET(peer_socket, &writes))
				{
					
						control_ob.mydata_to_server(pl1, ship_id, shipdata, newBullets, mutx);
						data2.packet_id = frame_number;
						data2.shipdata_forServer = shipdata;
						//sending the data
						if (shipdata.size_navigation > 0)
						{
							cout << "\n navigation called at frame number==>" << frame_number;
							
						}



						int bytes = send(peer_socket, (char*)&data2, sizeof(data2), 0);
						while (bytes < sizeof(data2))
						{
							bytes += send(peer_socket, (char*)&data2 + bytes, sizeof(data2) - bytes, 0);
						}
					
						if (bytes < 1)
						{
							break;
						}

				}
			}
			avg_send += sending.getElapsedTime().asSeconds();
			
		}
			
	}
	cout << "\n average receiving time==>" << avg_recv / c;
	cout << "\n average processing time==>" << avg_processing / c;
	cout << "\n average sending time==>" << avg_send / c;

	CLOSESOCKET(peer_socket);
	WSACleanup();

}

int main()
{
	//extracting the data
	
	
	SOCKET sock = connect_to_server();
	
	//receiving the startupinfo data
	Startup_info_client start_data;
	memset((void*)&start_data, 0, sizeof(start_data));
	int bytes = recv(sock, (char*)&start_data, sizeof(start_data),0);
	while (bytes < sizeof(start_data))
	{
		bytes += recv(sock, (char*)&start_data + bytes, sizeof(start_data) - bytes, 0);
	}
	cout << "\n received bytes are==>" << bytes;
	cout << "\n my ship id is==>" << start_data.ship_id;
	cout << "\n my pos is=>" << start_data.starting_pos.r << " " << start_data.starting_pos.c;
	const int no_of_players = start_data.no_of_players;
	cout << "\n number of players==>" << no_of_players;

	ship *player = new ship[no_of_players];

	Control control;
	//creating an object of class Mutex: this object will be passed to every class using mutex
	Mutex mutx;

	//sf::RenderWindow window(sf::VideoMode(1920,1080),"testing window");
	int code[rows][columns];
	List<Greed::coords> opaque_coords;//coordinates of the land 
	Map map1(0);
	map1.initialize_map(code, opaque_coords);
	control.opaque_coords = opaque_coords;
	Greed ob;
	List<Greed::vertex>& l = ship::getGlobalMapref(2369);//no need to change this line
	ob.getMap(code, rows, columns, l); // here globalMap is set
	//initializing globalMap in ship class
	deque<shipInfo> silist;
	List<Greed::coords> l1;
	


	//player[1].updateCost(Map::coords(10,0), 50);

	//player[1].localMap[10 * columns + 0].b.cost = 50;
	deque<ship*> slist;

	for (int i = 0; i < no_of_players; i++)
	{
		silist.push_back(shipInfo(&player[i]));
		slist.push_back(&player[i]);
	}
	//generating the coords for spawning the ship
	vector<Greed::coords> spawn;//spawning position for ships having different configurations
	spawn.push_back(start_data.starting_pos);
	
	player[start_data.ship_id].initialize_player(start_data.ship_id, "hawk", &mutx, silist, code, 5, spawn[0]);//last element is the tile pos of the player

	for (int i = 0; i < no_of_players; i++)
	{
		if (i != start_data.ship_id)
		{
			player[i].mutx = &mutx;
			player[i].ship_id = i;
			player[i].mutex_id = i;
			player[i].shipInfoList = silist;
		}
	}

	control.setShipList(slist, 2369);
	// player[1].localMap[10 * columns + 0].b.cost = 50;
	//making the cannon lists:
	//let there be 1 cannon only 
	List<Greed::cannon> cannon_list;
	for (int i = 1; i <= 3; i++)
	{
		cannon_list.add_rear(Greed::cannon());

	}

	cannon_list[0].initializer(0, Greed::coords(5, 11));
	cannon_list[1].initializer(1, Greed::coords(3, 6));
	cannon_list[2].initializer(2, Greed::coords(7, 8));

	control.cannon_list = cannon_list;
	ship::cannon_list = cannon_list;
	
	graphics cg;
	cg.callable_client(start_data.ship_id,&mutx, code, map1, sock,player[start_data.ship_id]);
	

}


