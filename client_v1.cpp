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
#define MAX_LENGTH 1000


#include "online_lib2.hpp"
#include "online_lib2.cpp"
#include "hawk.cpp"
#include<ctime>
#include<chrono>
#include<conio.h>
#include<regex>
/*
* what am i doing here?sssssssssssssssss
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
void GreedMain(ship& ob);
string username = "username";
string password = "password";
string game_token;
int my_id;//id of the player in the game
string ip_address;//ip to which the client will connect to..
int mode;
SOCKET sending_socket;//socket to send data to the server
//peer_socket is for recving data from the server

deque<recv_data> input_data;//input data from the server to the client
deque<send_data> terminal_data;



long game_tick = 1;//this is the game tick of the server not the client

bool SEND(SOCKET sock, char* buff, int length)
{
	/*overload of send function of UDP having the mechanism of resending and ack*/
	fd_set master;
	FD_ZERO(&master);
	FD_SET(sock, &master);
	fd_set read;
	FD_ZERO(&read);
	chrono::steady_clock::time_point now = chrono::steady_clock::now();
	
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int b = send(sock, buff, length, 0);
	if (b < 1)
	{
		cout << "\n cannot send the bytes.." << GETSOCKETERRNO();
		return false;
	}
	while (1)
	{
		read = master;
		select(sock + 1, &read, 0, 0, &timeout);
		if (FD_ISSET(sock, &read))
		{
			int ack;
			int bytes = recv(sock, (char*)&ack, sizeof(ack), 0);
			if (ack == 1)
			{
				return true;
			}
		}
		auto end = chrono::steady_clock::now();
		chrono::duration<double> iteration_time = chrono::duration_cast<chrono::duration<double>>(end - now);
		if (iteration_time.count() > 1)
		{
			//resend the packet
			int bytes = send(sock, buff, length, 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the bytes again=>" << GETSOCKETERRNO();
				return false;
			}
			
			now = chrono::steady_clock::now();//changing the time since the last message sent
		}
		
	}
}
bool RECV(SOCKET sock, char* buff, int length)
{
	//cout << "\n from RECV: just entered into the function..";
	/*overload for UDP recv, to recv the data carefully*/
	while (1)
	{
		int b = recv(sock, buff, length, 0);
		//cout << "\n from RECV: received some bytes;";
		if (b > 1)
		{
			int ack = 1;
			int bytes = send(sock, (char*)&ack, sizeof(ack), 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the bytes. . " << GETSOCKETERRNO();
			}
			
			return true;
		}
	}

}
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
	char port_str1[10];
	sprintf(port_str1, "%d", (port + 2));

	//printf("Configuring remote address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo* peer_address;
	struct addrinfo* peer_address1;
	
	if (getaddrinfo(ip_address.c_str(), port_str, &hints, &peer_address)) 
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (getaddrinfo(ip_address.c_str(),port_str1, &hints, &peer_address1))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);//for receiving data from the game server

	sending_socket= socket(peer_address1->ai_family,
		peer_address1->ai_socktype, peer_address1->ai_protocol);//for sending data to the game server
	
	if (!ISVALIDSOCKET(socket_peer)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (!ISVALIDSOCKET(sending_socket)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	if (connect(sending_socket, peer_address1->ai_addr, peer_address1->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	//sending the greet_client object to the game server, with my code and credentials.
	
	return socket_peer;

}
int prev_server_tick = 0;//this is to check the client does not accept any old packets from the server
void data_recver(SOCKET socket_listen,Mutex *m)
{
	int found = 0;
	int bytes;
	sf::Clock clock;
	double et = 0;
	int count = 0;
	while (1)
	{
		et+=clock.restart().asSeconds();
		if (et > 1)
		{
			//cout << "\n the count of packet is==>" << count;
			et = 0;
			count = 0;
		}
		
		recv_data ob;
		int recv_bytes = 0;
		char comp[sizeof(ob)];
		int checker=0;
		if (found == 0)
		{
			recv(socket_listen, (char*)&checker, sizeof(checker), 0);
		}
		if (checker == 1 || found == 1)
		{
			while (recv_bytes < sizeof(ob))
			{
				char buffer[1000];
				found = 0;
				 bytes = recv(socket_listen, (char*)&buffer, sizeof(buffer), 0);
				if (bytes < 1)
				{
					//cout << "\n cannot recv the bytes==>" << GETSOCKETERRNO();
					continue;
				}
				if (bytes == 4)
				{
					int check;
					memcpy(&check, buffer, sizeof(int));
					if (check == 1)
					{
						cout << "\n problem occured breaking...";
						found = 1;
						break;
					}
				}
			
					memcpy(comp + recv_bytes, buffer, bytes);
					recv_bytes += bytes;
					//cout << "\n recved bytes from the server=>" << bytes << " " << ob.arr[100];
					auto now = std::chrono::system_clock::now();
					auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
					auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
					auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
					auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

					//cout << "\n recved data from the server at the time==> " <<
					//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
				

			}
			memcpy(&ob, comp, sizeof(ob));
			if (ob.st == 100 && ob.end == 101)
			{
				prev_server_tick = ob.packet_id;
				unique_lock<mutex> lk(m->recv_terminal);
				input_data.push_back(ob);
				count++;
				m->cond_input_terminal.notify_all();
			}
			else
			{
				cout << "\n faulty data";
			}
		}
		//Sleep(10);
	}
}
void data_send(Mutex* m,deque<ship*> &pl1)
{
	sf::Clock clock;
	double ep = 0;
	int count = 0;
	while (1)
	{
		ep += clock.restart().asSeconds();
		if (ep > 1)
		{
			ep = 0;
			//cout << "\n packets sent are==>" << count << endl;
			count = 0;
		}
		
		unique_lock<mutex> lk(m->send_terminal);
		m->cond_terminal.wait(lk, [] {return !terminal_data.empty(); });
		//terminal_data queue has data now make its copy and clear the queue
		deque<send_data> data;
		for (int i = 0; i < terminal_data.size(); i++)
		{
			data.push_back(terminal_data[i]);

		}
		terminal_data.clear();//clearing the queue
		lk.unlock();
		
		for (int i = 0; i < data.size(); i++)
		{
			//send the data to the client
			data[i].st = 100;
			data[i].end = 101;
			/*
			if (data[i].shipdata_forServer.size_navigation > 0)
			{
				auto now = std::chrono::system_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
				auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
				auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());
				cout << "\n from client sending part, sent for the set Path function==>" << hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;

			}
			*/
			int limit = 1;//default number of times the pcket will be sent 
			if (data[i].shipdata_forServer.size_bulletData > 0 || data[i].shipdata_forServer.size_navigation > 0 || data[i].shipdata_forServer.size_update_cost > 0 || data[i].shipdata_forServer.size_upgrade_data > 0)
			{
				limit = 5;
			}
			for (int k = 1; k <= limit; k++)
			{
				send_data ob = data[i];
				//cout << "sending the packet==>" << ob.packet_id;
				char buffer[sizeof(data[i])];
				memcpy(buffer, &ob, sizeof(ob));

				int sent_bytes = 0;
				//sending that starting a new packet
				int sending_new = my_id;//sending my id to the person

				send(sending_socket, (char*)&sending_new, sizeof(sending_new), 0);
				while (sent_bytes < sizeof(ob))
				{
					int fuck = sizeof(ob) - sent_bytes;
					int bytesToSend = min(MAX_LENGTH, fuck);
					if (ob.st != 100 || ob.end != 101)
					{
						cout << "\n sending the wrong data from the client side";
					}
					int bytes = send(sending_socket, buffer + sent_bytes, bytesToSend, 0);
					if (bytes < 1)
					{
						//cout << "\n cannot send bytes to the client==>" << GETSOCKETERRNO();
					}
					else
					{
						sent_bytes += bytes;

						auto now = std::chrono::system_clock::now();
						auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
						auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
						auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
						auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

						//cout << "\n sent data to the client at==> " <<
						  // hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
					}
				}
			}
			auto now = std::chrono::system_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
			auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
			auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
			auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

			//cout << "\n sent data to the server packet id==>" <<ob.packet_id<<" at=>"<<
			 // hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
			//cout << "\n sent bytes to==>" <<data[i].first;
			
			count++;
		}
	}
}
bool find_id(deque<upgrade_data>& arr, int id)
{
	for (auto it : arr)
	{
		if (it.id == id)
		{
			return false;
		}
		
	}
	return true;
}
bool find_bullet_id(deque<bullet_data>& arr, int id)
{
	for (auto it : arr)
	{
		if (it.bullet_id == id)
		{
			return false;
		}
	}
	return true;
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
	int next_frame = 0;
	control1 control_ob;
	//THIS code is for updating the fuel of the ship

	GameState data;
	long total_time = 0;
	long double frame_number = -1;//this is for packet id that the client will send to the server

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	fd_set master_read;
	FD_ZERO(&master_read);
	FD_SET(peer_socket, &master_read);
	
	fd_set master_write;
	FD_ZERO(&master_write);
	FD_SET(sending_socket, &master_write);

	
	fd_set reads;
	fd_set writes;
	FD_ZERO(&reads);
	FD_ZERO(&writes);

	vector<Greed::bullet> newBullets;
	cout << "\n from now on the statements will be printed of your algorithm..\n";
	thread t(GreedMain, ref(player));
	t.detach();
	int frame_no = 0;
	double avg_nav_req = 0;
	int total_frames = 0;
	int tot = 0;
	int frame_rate = 0;
	double check_time = 0;
	
	int cc = 0;
	double avg_recv = 0;
	double avg_send = 0;
	double avg_processing = 0;
	int prev_pack = 0;

	int previous_packet = -1;
	//accepting the testing message from the servser

	int send_count = 0;
	double avg_send_count = 0;
	int total_frames1 = 0;

	int threshold_health = -1;
	int threshold_ammo = -1;
	int threshold_fuel = -1;
	//to check if the values of these things have changed or not
	int nav_data_count = 0;
	int nav_data_once = 0;
	int b_data_count = 0;
	int b_data_once = 0;
	int c_data_count = 0;
	int c_data_once = 0;
	int u_data_once = 0;
	int u_data_count = 0;
	
	thread t1(&data_recver, peer_socket, mutx);//starting the thread which will receive the data from the server
	t1.detach();

	thread t3(data_send, mutx,std::ref(pl1));//starting the thread which will send the data to the server
	t3.detach();
	int avg_input = 0;
	int total_count = 0;

	int current_health=pl1[ship_id]->getCurrentHealth();
	int current_fuel=pl1[ship_id]->getCurrentFuel();
	int current_ammo = pl1[ship_id]->getCurrentAmmo();
	sf::Clock clock1;
	double ep = 0;
	int prev_tick = 0;
	int effective_frame_rate = 0;
	int wait_tick = 0;
	control1 cont;//object of control1 class

	int nav_res_count = 1;//to keep the count in order to wait before resending anything.
	int bullet_res_count = 1;

	int no_bullet_resend = 0;//to keep a count on number of bullets that were resent

	int no_of_bullets = 0;
	int no_of_times = 0;
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
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			gameOver = true;
			break;
		}
		sf::Time tt = clock1.restart();
		ep += tt.asSeconds();
		if (ep > 1)
		{
		//	cout << "\n effective frame rate=>" << effective_frame_rate;
			frame_rate = 0;
			effective_frame_rate = 0;
			ep = 0;
		}
		//next_frame = ep * 60;
		if (next_frame != cur_frame)//under this frame rate is stable
		{
			
			frame_rate++;
			cc++;
			reads = master_read;
			writes = master_write;
			
			cur_frame = next_frame;
			//for handling the ship
			total_time=game_tick;
			
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
			
			
			unique_lock<mutex> lock(mutx->recv_terminal);
			int gone = 0;
			mutx->cond_input_terminal.wait(lock, [&] { 
				if (input_data.size() == 0)
				{
					/*
					wait_tick++;
					if (wait_tick == 100)
					{
						wait_tick = 0;
						//game_tick++;
						cout << "\n used";
						return true;
					}
					*/
					
					return false;
				}
				return true;
				});
			//recving the data from the server here
					if (input_data.size() == 1)//case when only one packet is there from the server
					{
						
						avg_input += input_data.size();
						gone = 1;
						recv_data data1 = input_data[0];
						input_data.pop_front();
						lock.unlock();
						
						game_tick = data1.packet_id;
						//cout << "\n recved packet with id=>" << data1.packet_id;
						//cout << "\n packet id==>" << data1.packet_id;
						//cout << "\n packet id=>" << data1.packet_id;
						control_ob.packet_to_pl(data1.shipdata_exceptMe, data1.s1, ship_id, pl1);
						control_ob.packet_to_me(data1.shipdata_forMe, ship_id, pl1);
					//	cout << "\n health is==>" << data1.shipdata_forMe.health;

						previous_packet = data1.packet_id;
						gameOver = data1.gameOver;
						auto now = std::chrono::system_clock::now();
						auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
						auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
						auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
						auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());
						
						//cout << "\n recved data from the server terminal =>" << data1.packet_id << " at the time==> " <<
							//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
					}
					else//enters here only when the size of input data is more than one, then merging is needed
					{
						//taking down all the packets and starting the merging
						deque<recv_data> temp_recv;
						for (int i = 0; i < input_data.size(); i++)
						{
							temp_recv.push_back(input_data[i]);
						}
						input_data.clear();
						lock.unlock();
						
						game_tick = temp_recv[temp_recv.size() - 1].packet_id;//setting the packet id to the latest packet in the queue
						//starting to unfolding the packet
						control_ob.packet_to_pl(temp_recv[temp_recv.size()-1].shipdata_exceptMe, temp_recv[temp_recv.size() - 1].s1, ship_id, pl1);//for this just send the latest packet that we have

						control_ob.packet_to_me(temp_recv[temp_recv.size() - 1].shipdata_forMe, ship_id, pl1);//here we have to merge some data
						//adding the previous data only in hit_bullet
						for (int i = temp_recv.size() - 2; i >= 0; i--)
						{
							for (int j = 0; j < temp_recv[i].shipdata_forMe.size_hit_bullet; j++)
							{
								Greed::bullet b;
								cont.data_to_bullet(b, temp_recv[i].shipdata_forMe.hit_bullet[i]);
								
								pl1[ship_id]->hit_bullet.push_front(b);
							}
						}
					}
				//check if navigation and firing commands reached the server properly, if not then resend them
					for (int i = 0; i < resend_navigation.size(); i++)
					{

						pl1[ship_id]->nav_data_final.push_back(resend_navigation[i]);
					}


								
						for (int i = 0; i < resend_bullet.size() && i < 5; i++)
						{
							unique_lock<mutex> lk(mutx->m[ship_id]);
							//cout << resend_bullet[i].first.bullet_id << " ";
							//cout << "\n server fire=>" << pl1[ship_id]->server_fire << " " << " client fire=>" << pl1[ship_id]->client_fire;
							if (find_bullet_id(pl1[ship_id]->bullet_info,resend_bullet[i].first.bullet_id) && pl1[ship_id]->client_fire > pl1[ship_id]->server_fire)
							{
							//	cout << "\n reattempting fire command for=>" << resend_bullet[i].first.bullet_id;

								pl1[ship_id]->bullet_info.push_front(resend_bullet[i].first);
								resend_bullet[i].second++;
								no_bullet_resend++;

							}
							
							else if (pl1[ship_id]->client_fire < pl1[ship_id]->server_fire)
							{
							//	cout << "\n server fire=>" << pl1[ship_id]->server_fire << " " << " client fire=>" << pl1[ship_id]->client_fire;
							}
						}
						bullet_res_count = 1;
					
					if (resend_navigation.size() > 0)
					{
						nav_res_count++;
					}
					if (resend_bullet.size() > 0)
					{
						bullet_res_count++;
					}
					if(pl1[ship_id]->upgrade_queue.size()>0)
					//cout << "\n upgrade queue is==>";
					for (int i = 0; i < pl1[ship_id]->upgrade_queue.size(); i++)
					{
						
					
							//check if it has to be resent or not?
							
								if (find_id(pl1[ship_id]->udata,pl1[ship_id]->upgrade_queue[i].udata.id))//in this case retrasmit, without deleting it from the queue
								{
									pl1[ship_id]->udata.push_back(pl1[ship_id]->upgrade_queue[i].udata);
									//cout << "\n reattempting fire with the id=>" << pl1[ship_id]->upgrade_queue[i].udata.id;
								}
						
						//	cout << pl1[ship_id]->upgrade_queue[i].type << " ";
							
						
					}
						

						unique_lock<mutex> lk1(pl1[ship_id]->mutx->m[ship_id]);
						if (current_health < pl1[ship_id]->health)
						{
							pl1[ship_id]->lock_health = 0;
							cout << "\n lock unlocked manually for health";
							
						}
						if (current_ammo < pl1[ship_id]->ammo)
						{
							pl1[ship_id]->lock_ammo = 0;
							cout << "\n lock unloced manually for ammo";
						}
						if (current_fuel < pl1[ship_id]->fuel)
						{
							pl1[ship_id]->lock_fuel = 0;
						}
						current_health = pl1[ship_id]->health;
						current_ammo = pl1[ship_id]->ammo;
						current_fuel = pl1[ship_id]->fuel;

						lk1.unlock();


						avg_recv += recvt.getElapsedTime().asSeconds();

						//std::time_t result = std::time(nullptr);
						//cout << "\n----------------------------------------------------------";
						//cout << "\n time=>"<<std::localtime(&result)->tm_hour<<":"<< std::localtime(&result)->tm_min<<":"<< std::localtime(&result)->tm_sec << " client frame = >" << total_time << " " << " received frame = >" << data1.packet_id;
						if (gameOver || pl1[ship_id]->died == 1)
						{
							//cout << "\n breaking because the game is over(as told by the server, or the ship has died)";
							break;
						}
						if (!gameOver)
						{
							//using nav_data
							sf::Clock process;
							process.restart();
							unique_lock<mutex> sigma(mutx->m[ship_id]);
							if (pl1[ship_id]->nav_data.size() > 0)
							{
								for (int i = 0; i < pl1[ship_id]->nav_data.size(); i++)
								{
									pl1[ship_id]->nav_data_final.push_back(pl1[ship_id]->nav_data[i]);
								}
								pl1[ship_id]->nav_data.clear();
							}
							sigma.unlock();

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
													cout << "\n pushing the event that a ship is in my radius...";
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
						if (pl1[1]->isShipInMyRadius(0))
						{
							//cout << "\n the ship is in my radius at the game tick==>" << game_tick;
							auto now = std::chrono::system_clock::now();
							auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
							auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
							auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
							auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

							//cout <<  " at the time==> " <<
							//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
						}

						if (pl1[ship_id]->died == 0)//send the data only if the ship is alive
						{
							send_data data2;
							shipData_forServer shipdata;
							std::memset((void*)&shipdata, 0, sizeof(shipdata));
							std::memset((void*)&data2, 0, sizeof(data2));


							//if (threshold_health==pl1[ship_id]->threshold_health&&threshold_ammo==pl1[ship_id]->threshold_ammo&&threshold_fuel==pl1[ship_id]->threshold_fuel&& pl1[ship_id]->nav_data_final.size() == 0 && pl1[ship_id]->bullet_info.size() == 0 && pl1[ship_id]->udata.size() == 0 && pl1[ship_id]->map_cost_data.size() == 0)

							threshold_ammo = pl1[ship_id]->threshold_ammo;
							threshold_fuel = pl1[ship_id]->threshold_fuel;
							threshold_health = pl1[ship_id]->threshold_health;
							for (int i = 0; i < pl1[ship_id]->bullet_info.size() && i<15; i++)
							{
								//cout << "\n sending firing data at==>" << game_tick;
							}
							for (int i = 0; i < pl1[ship_id]->nav_data_final.size(); i++)
							{
								cout << "\n sending navigation data at==>" << game_tick;
							}

							no_of_bullets += pl1[ship_id]->bullet_info.size();
							no_of_times++;
							control_ob.mydata_to_server(pl1, ship_id, shipdata, newBullets, mutx,nav_res_count,bullet_res_count,no_bullet_resend);
							
						
							
							frame_number++;
							game_tick++;
							effective_frame_rate++;
							//cout << "\n in callable sending the packet==>" << game_tick;
							data2.packet_id = game_tick;
							data2.shipdata_forServer = shipdata;
							data2.user_cred = user_credentials(username, password);
							for (int i = 0; i < data2.shipdata_forServer.size_upgrade_data; i++)
							{
								if (data2.shipdata_forServer.udata[i].type == 1)
								{
									//cout << "\n sending for health to the sever at the health level=>" << pl1[ship_id]->getCurrentHealth();
								}
							}
							if (data2.shipdata_forServer.size_navigation > 0 && my_id==1)
							{
								//cout << "\n sent navigation request to the server==>"<<game_tick;
								auto now = std::chrono::system_clock::now();
								auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
								auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()) % 60;
								auto mins = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()) % 60;
								auto hours = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch());

								//cout << "\n at the time==> " <<
								//hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() << endl;
							}
							send_count++;
							unique_lock<mutex> lk(mutx->send_terminal);
							terminal_data.push_back(data2);
							lk.unlock();
							mutx->cond_terminal.notify_one();


						}
						avg_send += sending.getElapsedTime().asSeconds();
						total_count++;
					
		}
		
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - begin);
		elapsed_time += time_span.count();
		if (elapsed_time > 1)
		{
			elapsed_time = 0;
			//cout << "\n frame rate is==>" << frame_rate;
			//frame_rate = 0;
			total_frames1++;
		}
		
		next_frame = elapsed_time * 80;
		
			
	}
	cout << "\n number of bullets coming per time is==>" << (double)no_of_bullets / no_of_times;
	CLOSESOCKET(peer_socket);
	CLOSESOCKET(sending_socket);
	WSACleanup();

	while(1)
	{ }

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

string input_password()
{
	const int MAX_PASSWORD_LENGTH = 20;
	char password1[MAX_PASSWORD_LENGTH + 1]; // +1 for null terminator
	const char BACKSPACE = 8;
	const char ENTER = 13;
	std::cout << "Enter your password==>";
	char ch;
	int i = 0;
	while (true) {
		ch = _getch();

		if (ch == ENTER) {
			password1[i] = '\0'; // Null-terminate the password
			break;
		}
		else if (ch == BACKSPACE) {
			if (i > 0) {
				std::cout << "\b \b"; // Move cursor back, overwrite with space, move back again
				--i;
			}
		}
		else if (i < MAX_PASSWORD_LENGTH - 1) {
			std::cout << '*';
			password1[i++] = ch;
		}
	}

	return string(password1);
}
string checkEmail()
{
	string email;
	bool true_mail = false;
	while (!true_mail)
	{
		cin >> email;
		std::regex pattern("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

		// Check if the email matches the pattern
		true_mail= std::regex_match(email, pattern);
		if (!true_mail)
		{
			cout << "\n please enter a valid email==>";
		}
	}
	return email;
	//now checking the email

}
bool login_function(SOCKET lobby_socket);
void createAccount(SOCKET lobby_socket)
{
	int status_code = 0;
	cout << "\n You don't seem to have an account with us...let's create you one...\n Enter the details:\n what do you want to be called? =>";
	while (status_code != 1)
	{
		cin >> username;
		//checking if this username is available or not
		user_credentials ob(2, username, "");
		int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
		bytes = recv(lobby_socket, (char*)&status_code, sizeof(status_code), 0);
		if (status_code == -1)
		{
			cout << "\n Oops..This username is taken, try something else? Enter another username=>";
		}

	} 
	//now we are having our username set
	cout << "\n Wow! You have picked youself a deserving name...\n";
	password = input_password();
	cout << "\n Just a few more things...\n Enter you email address ( Enter a valid email, because all your in-game credits and rewards will be intimidated to you on that email)=>";
	string email = checkEmail();
	cout << "\n which country do you come from?";
	string country;
	cin >> country;
	cout << "\n what is your designation? eg. student, working etc.==>";
	string role;
	cin >> role;
	user_credentials ob(1, username, password, email, role, country);
	send(lobby_socket, (char*)&ob, sizeof(ob), 0);
	login_function(lobby_socket);

}
bool login_function(SOCKET lobby_socket)
{
	cout << "\n enter you username==>";
	cin >> username;
	password = input_password();
	user_credentials ob(0, username, password);
	int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
	if (bytes < 0)
	{
		cout << "\n could not send the credentials to the lobby server=>" << GetLastErrorAsString();
	}
	int status_code = 0;
	bytes = recv(lobby_socket, (char*)&status_code, sizeof(status_code), 0);
	//if everything goes right the status code should be 1
	if (bytes < 1)
	{
		cout << "\n error in recving bytes==>" << GETSOCKETERRNO();
	}
	cout << "\n the status code is==>" << status_code;
	while (status_code != 1)
	{
		if (status_code == 2)
		{
			//the password of the user is wrong, so promt the user to input the password again
			cout << "\n the password you entered is wrong...";
			password = input_password();
			user_credentials ob(0, username, password);
			int bytes = send(lobby_socket, (char*)&ob, sizeof(ob), 0);
			status_code = 0;
			bytes = recv(lobby_socket, (char*)&status_code, sizeof(status_code), 0);

		}
		if (status_code == -1)
		{
			createAccount(lobby_socket);
		}
	}
	return true;

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
	
	while (connect(lobby_socket, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		cout << GetLastErrorAsString();
		//return 1;
	}
	freeaddrinfo(bind_address);
	cout << "\n connected with the lobby server..";
	//sending hi to the server
	//sending my credentials to the server
	cout << "\n Enter 1 to login ( if you have a username and password) \n Enter 2 to signup (if you don't have any username)";
	int choice = 0;
	cin >> choice;
	if (choice == 1)//the user wants to login
	{
		login_function(lobby_socket);
		
		
	}
	else if (choice == 2)
	{
		createAccount(lobby_socket);
	}
	



	

	cout << "\n waiting for game token and port of the game server...";
	
	server_startup start;
	int port;
	int bytes = recv(lobby_socket,(char*)&start, sizeof(start), 0);
	if (bytes < 1)
	{
		cout << "\n error in recv bytes from lobby server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	port = start.port;
	game_token = start.token;
	cout << "\n received port and token from the lobby server..";
	return port;
}

int main(int argc,char* argv[])
{
	//extracting the data
	
	cout << "\n enter the ip address of the server==>";
	cin >> ip_address;
	cout << "\n enter the mode in which you want to run this client.. press 1. for client with display unit.\n otherwise press 2==>";
	cin >> mode;
#if defined(_WIN32) 
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		fprintf(stderr, "Failed to initialize.\n");
		cout << "\n winsock not found.";
		return 1;
	}
#endif
	int port = 0;//port is of the game server which the lobby server will return.
	SOCKET socket_listen = 0;
	 port=connect_to_lobby_server();
	socket_listen = connect_to_server(port);   //no connection is established here only the socket initialization is done here
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
		
	/*
	cout << "\n waiting to recv the data before going for tcp ";
	int st = 0;
	int recvy = recv(socket_listen, (char*)&st, sizeof(st),0);
	if (st != 1)
	{
		cout << "\n found the wrong data";
	}
	*/

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* bind_address;
	int port2 = port + 3;
	char port2_str[10];
	sprintf(port2_str, "%d", port2);
	//cout << "\n port of the server2 is==>" << port2_str;
	getaddrinfo(ip_address.c_str(),port2_str, &hints, &bind_address);
	cout << "\n tcp connection port is=>" << port2_str;
	SOCKET tcp_socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(tcp_socket))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		//return 1;
	}
	int enableKeepAlive = 1;
	setsockopt(tcp_socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&enableKeepAlive, sizeof(enableKeepAlive));
	while (::connect(tcp_socket, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		cout << GetLastErrorAsString();
		//return 1;
	}
	char ip[100];
	char cport[100];
	getnameinfo(bind_address->ai_addr,bind_address->ai_addrlen, ip, sizeof(ip), cport, sizeof(cport), NI_NUMERICHOST | NI_NUMERICSERV);
	cout << "\n address of the tcp server=>" << ip;
	cout << "\n port of the tcp server=>" << cport;
	freeaddrinfo(bind_address);
	cout << "\n tcp connected with the client..";
	//cout << "\n connected with the game server using tcp. ";
	int msg;
	if (mode == 1)
		msg = 0;//0 means client_v1_process
	else if (mode == 2)
	{
		msg = 2;
	}
	greet_client gc;
	gc.code = msg;
	gc.user_cred = user_credentials(username, password);
	strcpy(gc.token, game_token.c_str());


	if (!send(tcp_socket, (char*)&gc, sizeof(gc), 0))
	{
		cout << "\n cannot send the code to the server=>" << GETSOCKETERRNO();
	}

	cout << "\n waiting for the game server to send me the id..";
	recv(tcp_socket, (char*)&my_id, sizeof(my_id), 0);
	cout << "\n id sent by the client is=>" << my_id;


	//starting the display unit
	if (mode == 1)
	{
		//added the thing that when the game overs, the client will break the loop and close the connection.
		char path_c[MAX_PATH];
		string path;
		if (GetCurrentDirectoryA(MAX_PATH, path_c) != 0)
		{
			path = path_c;
		}
		path += "\\client_v2_new.exe ";

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		//LPTSTR* arg = { "hello.exe" };
		//convert port to string and append in commandLine

		string port_str = to_string(port);
		string id = to_string(my_id);
		char cwd[256];

		string commandLine = path + port_str + " " + id + " " + username + " " + password + " " + game_token + " " + ip_address;
		//cout << "\n command line is==>" << commandLine;
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
	}
	//before stage2 we need to keep on sending heart beat messages to the server to keep the connection alive as long as some data is
	// not ready to recv from the server
	//stage 2.............
	//asking if everything is ready for stage 2?
	fd_set master;
	fd_set read;
	FD_ZERO(&master);
	FD_ZERO(&read);
	FD_SET(tcp_socket, &master);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	double elapsed_time = 0;
	while (1)
	{
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		read = master;
		select(tcp_socket + 1, &read, 0, 0, &timeout);
		if (FD_ISSET(tcp_socket, &read))
		{
			int st = 1;
			int b = recv(tcp_socket, (char*)&st, sizeof(st), 0);
				cout << "\n we are ready for stage 2==>" << st;
				break;
		}
		//sending heartbeat messages to the server here..............
		//chekcing if one second has elapsed or not 
		//these heartbeat message are used to  tell the server that the client is active so dont close the tcp connection
		//these heartbeat messages are set to be sent every 1 second of the tick
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - begin);
		elapsed_time += time_span.count();
		if (elapsed_time > 60)
		{
			elapsed_time = 0;
			//send the heartbeat message here
			char buff[100] = "hi terminal";
			int bytes = send(tcp_socket, buff, sizeof(buff), 0);
			if (bytes < 1)
			{
				cout << "\n cannot send the byes==>" << GETSOCKETERRNO();
			}
			cout << "\nsent heartbeat to the server";
			
		}
	}
	

	int bytes = send(tcp_socket, (char*)&my_id, sizeof(my_id), 0);
	
	if (bytes < 1)
	{
		cout << "\n error in sending my id to the game server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	cout << "\n sent the bytes to the server...";
	Startup_info_client start_data;
	memset((void*)&start_data, 0, sizeof(start_data));
	bytes = recv(tcp_socket, (char*)&start_data, sizeof(start_data), 0);
	if (bytes < 1)
	{
		cout << "\n error in receiving startup info from the game server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
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

	//waiting for the game server for the green signal to start the game;
	cout << "\n waiting for the server to send green signal to start the game....";
	int start = 0;
	bytes = recv(tcp_socket, (char*)&start, sizeof(start), 0);
	if (bytes < 1)
	{
		cout << "\n bytes recved are==>" << bytes;
		cout << "\n error in receiving startup info from the game server=>" << GetLastErrorAsString();
		cout << GETSOCKETERRNO();
	}
	cout << "\n starting the game==>" << start;

	


	//sending the final request to the client....for the udp socket

	for (int i = 1; i <= 10; i++)
	{
		
		int bytes = send(socket_listen, (char*)&my_id, sizeof(my_id), 0);
		if (bytes < 1)
		{
			cout << "\n error in sending the final bytes==>" << GetLastErrorAsString();
		}
	}
	cout << "\n waiting for the server to send the final flag to start the game...";
	int flag = 0;
	bytes = recv(tcp_socket, (char*)&flag, sizeof(flag), 0);
	if (bytes < 1)
	{
		cout << "\n cannot recv the final bytes from the server==>" << GetLastErrorAsString();
	}
	if (!flag)
	{
		cout << "\n the value of flag recved is==>" << flag;
		cout << "\n going to while loop....";

	}
	while (!flag) 
	{

	}
	

	CLOSESOCKET(tcp_socket);
	graphics cg;
	cg.callable_client(start_data.ship_id,&mutx, code, map1, socket_listen,player[start_data.ship_id]);
	//waiting for the child process to finish
	if (mode == 1)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		cout << "\n child completed";
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	
	
}


