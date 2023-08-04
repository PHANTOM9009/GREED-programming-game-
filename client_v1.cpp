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

string username = "username";
string password = "password";
string game_token;
int my_id;//id of the player in the game
string ip_address;//ip to which the client will connect to..
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

SOCKET connect_to_server(int port)//first connection to the server
{
	char port_str[10];
	// Convert port to string
	sprintf(port_str, "%d", port);

	printf("Configuring remote address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo* peer_address;
	
	if (getaddrinfo(ip_address.c_str(), port_str, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}


	printf("Remote address is: ");
	char address_buffer[100];
	char service_buffer[100];
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
		address_buffer, sizeof(address_buffer),
		service_buffer, sizeof(service_buffer),
		NI_NUMERICHOST | NI_NUMERICSERV);
	printf("%s %s\n", address_buffer, service_buffer);


	printf("Creating socket...\n");
	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	//sending the greet_client object to the game server, with my code and credentials.

	int msg = 0;//0 means client_v1_process
	greet_client gc;
	gc.code = msg;
	gc.user_cred = user_credentials(username, password);
	strcpy(gc.token, game_token.c_str());
	

	int bytes_sent = send(socket_peer,(char*)&gc,sizeof(gc), 0);

	printf("Sent %d bytes.\n", bytes_sent);

	cout << "\n waiting for the server to send me the id=>";
	int bytes = recv(socket_peer, (char*)&my_id, sizeof(my_id), 0);
	cout << "\n id sent by the client is=>" << my_id;
	return socket_peer;

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
	int cc = 0;
	double avg_recv = 0;
	double avg_send = 0;
	double avg_processing = 0;
	int prev_pack = 0;

	int previous_packet = -1;
	//accepting the testing message from the servser


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
			cc++;
		
			if (check_time > 1)
			{
				//cout << "\n frame rate is=>" << frame_rate;
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
				cout << "\n breaking because the ship is dead.";
				break;
			}
			sf::Clock recvt;
			recvt.restart();
			select(peer_socket + 1, &reads, 0, 0, &timeout);
			if (FD_ISSET(peer_socket,&reads))//socket is ready to read from
			{
				memset((void*)&data1, 0, sizeof(data1));
				bytes_recv = recv(peer_socket, (char*)&data1, sizeof(data1), 0);
				
				
				
				if (bytes_recv < 1)//connection is broken
				{
					//try to reconnect to the server
					cout << "\n average bullets are==>" << avg_bullet / no_of_times;
					cout << "\n jab aaye toh itne aaye=>" << avg_nav_req / total_frames;
					cout << "\n avg_nav_req over total frames=>" << avg_nav_req / tot;
					cout << "\n connection disconnected to the server..retrying to connect";
					//peer_socket = connect_to_server();
					cout << "\n connection is back.";
					cout << "\n server disconnected or the data is not able to be received==>" << GETSOCKETERRNO();
					CLOSESOCKET(peer_socket); 
					cout << "\n breaking due to not being able to recv the data";
					break;
				}
				//we have received the data.. now parse the data in original class structure.
				if (bytes_recv > 0 && strcmp(data1.token,game_token.c_str())==0)
				{
					//cout << "\n for frame=>" << total_time;w
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
					//cout << "\n packet id=>" << data1.packet_id;
					if (data1.packet_id - prev_pack > 1)
					{
					//	cout<<"\n packet loss=>"<<data1.packet_id - prev_pack;
					}
					prev_pack = data1.packet_id;
					//cout << "\n packet id==>" << data1.packet_id;
					control_ob.packet_to_pl(data1.shipdata_exceptMe, data1.s1, ship_id, pl1);
					control_ob.packet_to_me(data1.shipdata_forMe, ship_id, pl1);
					
					
					if (data1.shipdata_forMe.died == 1)
					{
						cout << "\n sent that i have died";
					}
					previous_packet = data1.packet_id;
					gameOver = data1.gameOver;
					auto now = std::chrono::system_clock::now();
					auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
					auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
					auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
					auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

					cout << "\n recved data from the client terminal =>" <<data1.packet_id << " at the time==> " <<
						hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
				}
				else
				{
					cout << "\n unverified server sending data..";

				}

				

			}
			avg_recv += recvt.getElapsedTime().asSeconds();
			
			//std::time_t result = std::time(nullptr);
		    //cout << "\n----------------------------------------------------------";
			//cout << "\n time=>"<<std::localtime(&result)->tm_hour<<":"<< std::localtime(&result)->tm_min<<":"<< std::localtime(&result)->tm_sec << " client frame = >" << total_time << " " << " received frame = >" << data1.packet_id;
			if (gameOver||pl1[ship_id]->died==1)
			{
				cout << "\n breaking because the game is over(as told by the server, or the ship has died)";
				break;
			}
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
						data2.user_cred = user_credentials(username, password);
						//sending the data
						if (shipdata.size_navigation > 0)
						{
							cout << "\n navigation called at frame number==>" << frame_number;
							
						}



						int bytes = send(peer_socket, (char*)&data2, sizeof(data2), 0);
											
						if (bytes < 1)
						{
							cout << "\n breaking because not being able to the send the data";
							//CLOSESOCKET(peer_socket);
							break;
						}
						else
						{
							auto now = std::chrono::system_clock::now();
							auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
							auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
							auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
							auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

							cout << "\n sent data to the server=>" << data1.packet_id << " at the time==> " <<
								hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
						}

				}
			}
			avg_send += sending.getElapsedTime().asSeconds();
			
		}
			
	}
	cout << "\n average receiving time==>" << avg_recv / cc;
	cout << "\n average processing time==>" << avg_processing /cc;
	cout << "\n average sending time==>" << avg_send / cc;

	CLOSESOCKET(peer_socket);
	WSACleanup();

}
std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}
int connect_to_lobby_server()
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* bind_address;
	getaddrinfo(ip_address.c_str(), "8080", &hints, &bind_address);
	SOCKET lobby_socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(lobby_socket))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		//return 1;
	}
	cout << "\n connecting with the lobby server.....";
	while (connect(lobby_socket, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		cout << GetLastErrorAsString();
		//return 1;
	}
	freeaddrinfo(bind_address);
	//sending hi to the server
	//sending my credentials to the server
	user_credentials ob(username, password);
	cout << "\n connected with the lobby server, now sending the bytes to the lobby server..";

	int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
	if (bytes < 0)
	{
		cout << "\n could not send the credentials to the lobby server=>" << GetLastErrorAsString();
	}
	cout << "\n sent the credentials to the lobby server..";
	cout << "\n waiting for game token and port of the game server...";
	
	server_startup start;
	int port;
	bytes = recv(lobby_socket,(char*)&start, sizeof(start), 0);
	if (bytes < 1)
	{
		cout << "\n error in recv bytes from lobby server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	port = start.port;
	game_token = start.token;
	cout << "\n received port and token from the lobby server=>" << port<<" "<<start.token;
	return port;
}

int main(int argc,char* argv[])
{
	//extracting the data
	cout << "\n enter the ip address of the server==>";
	cin >> ip_address;
#if defined(_WIN32) 
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		fprintf(stderr, "Failed to initialize.\n");
		return 1;
	}
#endif
	int port = 0;
	SOCKET socket_listen = 0;
	 port=connect_to_lobby_server();
	socket_listen = connect_to_server(port);   
	
	/*
	//added the thing that when the game overs, the client will break the loop and close the connection.
	char path_c[MAX_PATH];
	string path;
	if (GetCurrentDirectoryA(MAX_PATH, path_c) != 0)
	{
		path = path_c;
	}
	path += "\\client_v2_new.exe ";
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	//LPTSTR* arg = { "hello.exe" };
	//convert port to string and append in commandLine
	
	string port_str = to_string(port);
	string id = to_string(my_id);
	char cwd[256];

	string commandLine = path + port_str + " " + id + " " + username + " " + password + " " + game_token + " " + ip_address;
	cout << "\n command line is==>" << commandLine;
	char str[100];
	strcpy(str, commandLine.c_str());

	//char commandLine[] = "\"F:\\current projects\\GREED(programming game)\\GREED(programming game)\\client_v2_new.exe\" 8080";
	if (!CreateProcessA(NULL, str, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		cout << "Error creating process" << GetLastErrorAsString();

		return 1;
	}
	else
	{
		cout << "Process created";
		//return 0;
	}
	
	*/
	
	//receiving the startupinfo data
	Startup_info_client start_data;
	memset((void*)&start_data, 0, sizeof(start_data));
	 
	struct sockaddr_storage client_address;
	int client_length = sizeof(client_address);
	int bytes = recv(socket_listen, (char*)&start_data, sizeof(start_data), 0);
	
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
	cg.callable_client(start_data.ship_id,&mutx, code, map1, socket_listen,player[start_data.ship_id]);
	//waiting for the child process to finish
	
	/*
	WaitForSingleObject(pi.hProcess, INFINITE);
	cout << "\n child completed";
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	*/
	
	

}


