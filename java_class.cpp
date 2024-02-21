//this file contains the classes that have to be written in java





#define len 80
enum class map
{
	globalMap, localMap
};

enum class Direction
{
	NORTH, SOUTH, EAST, WEST, NORTH_WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NA = -1
};
enum class cannon_side
{
	FRONT, REAR, NA = -1
};
enum class ShipSide
{
	FRONT, REAR, MIDDLE, NA = -1
};
enum  class Entity
{
	WATER = 0, STORM = 1, LAND = 2, CANNON = 3, BONUS = 4, SHIP = 5, NA = -1
};
enum  class Bonus
{
	//define appropriate bonus here
	RESOURCE, HEALTH, BULLETS, GOLD, NA = -1
};

class attribute//attribute of the tile i.e what does the tile have: storm, bonus,land,cannons,ship ///  a list of the objects of this class will be returned.
{
protected:
	Entity entity;//type
	double cost;//in case of a water or storm only
	int ship_id;// if its a ship
	int cannon_id;//if its a cannon

	void update(double c, int sid, int cid, int key)
	{
		if (key == 2369)
		{

			cost = c;
			ship_id = sid;
			cannon_id = cid;

		}
	}
	void updateShipStatus(int sid, int key)
	{
		if (key == 2369)
		{
			ship_id = sid;
		}
	}
public:

	attribute()
	{

		cost = -1;
		ship_id = -1;
		cannon_id = -1;

	}
	attribute(Entity ent, double c, int sid, int cid)// ent=NA,c=-1,id=-1,b=NA if any of these doesnt exists
	{

		cost = c;
		ship_id = sid;
		cannon_id = cid;

	}


	double getCost()
	{
		return cost;
	}
	int getShipId()
	{
		return ship_id;
	}
	int getCannonId()
	{
		return cannon_id;
	}
	Entity getEntity()
	{
		return entity;
	}
	friend ship;

};
class Greed //main class for the game
{

public:
	class cannon;

	class abs_pos//holding the exact coordinates of the ship in the global world
	{
	public:
		double x;
		double y;
		abs_pos()
		{

		}
		abs_pos(double xx, double yy)
		{
			x = xx;
			y = yy;
		}
	};
	
	class coords//a container class for the  node // this class is wrapper for all the data to be used for a individual node
	{
	public:
		int r;
		int c;
		int cost;//cost of a tile
		double heuristic;
		Entity tile_id;//tells if it is a water , land and cannon only
		int cannon_id;//gives the cannon id if exists else -1
		Bonus bonus;//in case its a bonus tile
		///here are some of the new parameters.

		coords()
		{
			cost = 1;//default cost is 1
			cannon_id = -1;


		}
		coords(int rr, int cc, int co = 1)
		{
			r = rr;
			c = cc;
			cost = co;
		}
		coords(int rr, int cc, int co, Entity tile, int c_id, Bonus b)
		{
			r = rr;
			c = cc;
			cost = co;
			tile_id = tile;
			cannon_id = c_id;
			bonus = b;
		}
		bool operator==(coords two)
		{
			if (r == two.r && c == two.c)
				return true;
			else
				return false;
		}
		bool operator!=(coords two)
		{
			if (r != two.r || c != two.c)
			{
				return true;
			}
			else
				return false;
		}
	};
	//a fire class for ar
};
class shipInfo
{

	ship* ob;
	Greed::coords getCurrentTile_withoutMutex();

	Greed::coords getCurrentRearTile_withoutMutex();

public:

	shipInfo()
	{

	}
	shipInfo(ship* ob)
	{
		this->ob = ob;
	}
	int getTotalShips();
	int getShipId();
	int getShipRadius();
	double getCurrentHealth();
	double getCurrentGold();
	double getCurrentAmmo();
	double getCurrentResources();
	double getCurrentFuel();
	int  getDiedStatus();
	Direction getShipDirection();
	Greed::coords getCurrentTile();
	Greed::coords getCurrentRearTile();
	Greed::abs_pos getRealAbsolutePosition();

	friend class ship;


};
class Event
{
	static long  count;

	long event_id;
	int sid;//sid represents the id of the ship for which an event is created
	long double ttl;
public:
	long double timestamp;//time stamp of every event

	Event()
	{
		eventType = EventType::NA;
	}


	void setEventId()
	{
		event_id = count;
		count++;
	}

	Event(double timestamp, int event_id, int sid, double ttl)
	{
		this->timestamp = timestamp;
		this->event_id = event_id;
		this->sid = sid;
		this->ttl = ttl;
		eventType = EventType::NA;
		count++;
	}
	enum class EventType
	{
		ShipsInMyRadius,//0
		IamInShipRadius,
		IamInCannonRadius,
		CannonsInMyRadius,
		ShipFire,//when a ship fires
		CannonFire,//when a cannon fires 
		LowHealth,//when health is really low
		NoAmmo,
		LowAmmo,
		NoFuel,
		LowFuel,
		ShipCollision,//if a ship collides with another
		NA = -1
	}eventType;//object for the event class


	void initialize(double timestamp, EventType et, int sid)
	{
		this->timestamp = timestamp;
		this->sid = sid;

		eventType = et;
		ttl = timestamp;

	}
	class ShipCollision
	{
		vector<int> sid;
	public:
		ShipCollision() {}
		ShipCollision(vector<int>& s)
		{
			sid = s;
		}
		vector<int> getShipId()
		{
			return sid;
		}

		friend class ship;
		friend class graphics;
	};
	class LowFuel
	{
		double fuel;
	public:
		LowFuel() {}
		LowFuel(double f)
		{
			fuel = f;
		}
		double getFuel()
		{
			return fuel;
		}

		friend class ship;
		friend class graphics;
	};
	class LowAmmo
	{
		double ammo;
	public:
		LowAmmo() {}
		LowAmmo(double a)
		{
			ammo = a;
		}
		double getAmmo()
		{
			return ammo;
		}

		friend class ship;
		friend class graphics;
	};
	class LowHealth
	{

		double health;
	public:
		LowHealth() {}
		LowHealth(double h)
		{
			health = h;
		}
		double getHealth()
		{
			return health;
		}

		friend class ship;
		friend class graphics;
	};
	class CannonFire
	{
		unordered_map<int, Greed::bullet> cid;//vector of the cannnon ids
	public:
		CannonFire() {}
		CannonFire(unordered_map<int, Greed::bullet>& c)
		{
			cid = c;
		}
		unordered_map<int, Greed::bullet> getInfo()
		{
			return cid;
		}

		friend class ship;
		friend class graphics;
	};
	//write a loop till 100

	class ShipFire//SOME other ships fire at you
	{
		unordered_map<int, Greed::bullet> ob;//int is the id of the ship that fired the bullet
	public:
		ShipFire()
		{

		}
		ShipFire(unordered_map<int, Greed::bullet>& s)
		{
			ob = s;
		}
		unordered_map<int, Greed::bullet> getShipId()
		{
			return ob;
		}

		friend class ship;
		friend class graphics;
	};
	class Radius_cannon
	{
		vector<int> cid;
	public:
		Radius_cannon() {}
		Radius_cannon(vector<int>& c)
		{
			cid = c;
		}
		vector<int> getCannonId()
		{
			return cid;
		}

		friend class ship;
		friend class graphics;

	};
	class Radius_ship//this class contains data about the first two events in the list
	{

		vector<int> sid;//id of the ship in question
	public:

		Radius_ship()
		{

		}

		Radius_ship(vector<int>& s)
		{
			sid = s;
		}
		vector<int> getShipId()
		{
			return sid;
		}
		friend class ship;
		friend class graphics;

	};

	//declaring the objects 

	Radius_ship radiusShip;
	Radius_cannon radiusCannon;
	ShipFire shipFire;
	CannonFire cannonFire;
	LowHealth lowHealth;
	LowAmmo lowAmmo;
	LowFuel lowFuel;
	ShipCollision shipCollision;
	


};
class upgrade_data
{
public:
	int id;
	int type;
	/*
	* 0 for ammo
	* 1 for health
	* 2 for fuel
	*/
	int n;//upgrade by number

	upgrade_data() {
		id = -1;
	}
	upgrade_data(int t, int n)
	{
		type = t;
		this->n = n;
		id = -1;
	}
	friend class control1;
	friend class graphics;
};


class navigation
{
	int id;
	int type;
	/*
	* 0 for target type
	* 1 for sailing
	* 2 for chasing
	* 3 for aborting
	*/
	Greed::coords target;
	int s_id;//for target type and chasing
	int n;//number of tiles
	Direction dir;
public:
	navigation() {
		id = -1;
	}
	navigation(int t, Greed::coords tar, int sid, int nn, Direction d)
	{
		id = -1;
		type = t;
		target = tar;
		s_id = sid;
		n = nn;
		dir = d;
	}
	friend class graphics;
	friend class control1;

};

class bullet_data//new bullet data
{


public:
	int bullet_id;//unique id given to every bullet
	int type;
	/*
	* 0 for fireCannon or fire at ship
	* 1 for fireAtCannon or fire at cannon
	*/
	cannon can;
	int s_id;
	ShipSide s;
	int c_id;

	bullet_data()
	{
		bullet_id = -1;
	}
	bullet_data(int type, cannon can, int s_id, ShipSide s, int c_id)
	{
		bullet_id = -1;
		this->type = type;
		this->can = can;
		this->s_id = s_id;
		this->s = s;
		this->c_id = c_id;
	}
	friend class graphics;
	friend class control1;
};
deque<navigation> resend_navigation;//to store the navigation requests that has be resent
deque<pair<bullet_data, int>> resend_bullet;//firing data that has to be resent
class map_cost//class to send updateCost of the map tile data to the server over the network
{
public:

	double new_cost;
	Greed::coords ob;
	map_cost()
	{

	}
	map_cost(double n, Greed::coords o)
	{

		new_cost = n;
		ob = o;
	}
};

class shipData_exceptMe //updated data that the server will send for the client
{
	/*
	* This data will be sent by the server to every client. this contains data about ship except themselves
	* I will use other ship's data in following ways(I is the current ship)
	* 1. algorithm may use information about other ships
	* 2. to find the shipInRadius etc. events where we need some information about the position of the enemy ships
	*/

	int ammo;
	int died;
	Direction dir;
	int ship_id;
	bool isFiring;
	int score;

	double health;
	double gold;
	int motion;
	int fuel;
	int invisible;
	Greed::coords tile_pos_front;
	Greed::coords tile_pos_rear;
	Greed::abs_pos front_abs_pos;
	Greed::abs_pos rear_abs_pos;
	Greed::abs_pos absolutePosition;
	friend class control1;

};

class Greed::cannon//no need to lock this class with any mutex, no concurrency is performed here
{
public:
	int getCannonId()
	{
		return cannon_id;
	}
	double getCannonHealth()
	{
		return health;
	}
	Greed::coords getCannonPosition()
	{
		return tile;
	}
	double getCannonMoney()
	{
		return money;
	}
	bool isCannonDead()
	{
		return isDead;
	}

private:
	
	int cannon_id;
	double health;
	Greed::coords tile;
	Greed::abs_pos absolute_position;//abs pos of the middle of the cannon tile for the purpose of finding the slope.
	double money;
	bool isDead;//0 for dead 1 for alive
	int radius;
	bool busy;//to check if the cannon is currently busy
	double current_angle;//current angle of the cannon
	double req_angle;//angle required for the cannon 
	List<int> target_ships;//id of the ship that were targeted by the cannon, the last ship being the ship under attack by the cannon
	int current_ship;//id of the current attacking ship
	ShipSide ss;
	

	void filter(List<Greed::abs_pos>& ob);
	
	void initializer(int cid, Greed::coords tile)
	{
		cannon_id = cid;
		this->tile = tile;
		absolute_position = Greed::abs_pos((int)tile.c * len / 2, (int)tile.r * len / 2);
	}
	bool isShipInMyRadius(int s_id, ShipSide myside);
	
public:

	
	cannon()
	{
		cannon_id = -1;
		health = 500;
		money = 1000;
		isDead = false;
		radius = 2;
		busy = false;
		current_angle = 0;
		current_ship = -1;
	}

	

};
bool Greed::cannon::isShipInMyRadius(int s_id, ShipSide opponent_side)
{
	if (s_id >= 0)
	{
		Greed::coords ob = tile;
		Control obb;
		deque<ship*> l1;
		l1 = obb.getShipList(2369);


		Greed::coords position;
		if (opponent_side == ShipSide::FRONT)
		{
			position = l1[s_id]->getCurrenntTile_withoutMutex();
		}
		else if (opponent_side == ShipSide::REAR)
			position = l1[s_id]->getCurrentRearTile_withoutMutex();

		int c1 = position.c - radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int c2 = position.c + radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int r1 = position.r - radius;
		if (r1 < 0)
		{
			r1 = 0;
		}
		else if (r1 > rows)
		{
			r1 = rows - 1;
		}
		int r2 = position.r + radius;
		if (r2 < 0)
		{
			r2 = 0;
		}
		else if (r2 > rows)
		{
			r2 = rows - 1;
		}
		//checking if its in the radius or not;
		if (ob.c >= c1 && ob.c <= c2 && ob.r >= r1 && ob.r <= r2)
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
class Control//this class will control everything regarding the game
{
protected:
	mutex m1;//mutex lock for ship_list
	mutex m2;//mutex lock for cannon_list;
	mutex m3;//mutex for bonus
	mutex m4;//mutex for storm
	static deque<ship*> ship_list;
	static List<Greed::cannon> cannon_list;
	static List<Greed::coords> bonus;//list of the coords having bonus
	static List<Greed::coords> storm;//list of the coords having storm right now
	static List<Greed::coords> opaque_coords;

	void setBonusList(List<Greed::coords>& bonus)
	{
		unique_lock<mutex> lk(m3);
		this->bonus = bonus;
	}
	void setStormList(List<Greed::coords>& storm)
	{
		unique_lock<mutex> lk(m4);
		this->storm = storm;
	}
	List<Greed::coords> getOpaqueCoords()
	{
		return opaque_coords;
	}

	deque<ship*>& getShipList(int key)
	{
		if (key == 2369)
		{
			unique_lock<mutex> lk(m1);
			return ship_list;
		}


	}
	List<Greed::cannon>& getCannonList(int key)
	{
		if (key == 2369)
		{
			unique_lock<mutex> lk(m2);
			return cannon_list;
		}


	}
	void setShipList(deque<ship*>& ob, int key)//function to set the ship List
	{
		if (key == 2369)
		{
			unique_lock<mutex> lk(m1);
			ship_list = ob;

		}

	}


	List <Greed::coords>& getBonusList()
	{
		unique_lock<mutex> lk(m3);
		return bonus;
	}
	List<Greed::coords>& getStormList()
	{
		unique_lock<mutex> lk(m4);
		return storm;
	}


public:

	friend class ship;//declaring ship class as the friend such that mutex m1 and m2 can be used by its functions
	friend class graphics;

	friend class attribute;
	friend int main(int argc, char* argv[]);
	friend void filter();
	friend class Greed::cannon;
	friend void startup(int n, unordered_map<int, sockaddr_storage>& socket_id, int port);

};
class ship//this class will be used to initialize the incoming player and give it a ship. and then keep tracking of that ship
{


private:
	
	Event events;// a double sided queue of the events;
	
	static int total;//total ships
	
	int mutex_id;//id of the mutex to be used
	Greed map_ob;//to use the facilities of the class Greed
	int ship_id;//id of the ship
	

	vector<int> collided_ships;

	deque<upgrade_data> udata;

	std::chrono::high_resolution_clock::time_point current_time;
	chrono::steady_clock::time_point starting_time_limiter;//starting time when frame rate limiter started
	bool hector;//to check if frame_rate_limiter is running for the first time or not
	int current_frame_no;
	double elapsed_time;
	int frame_rate_limit;//value of the frame rate to be kept for the user function
	

public: //this will be public the user will be able to access this object freely
	//object 
	std::map<int, int> bullet_id_count;//used by the server
	std::map<int, int> upgrade_id_count;//to count the id's that have arrived to upgrade the data used by the server
	std::map<int, deque<map_cost>> upgrade_cost;//used by the client
	std::map<int, int> cost_id_count;//used by the server
	std::map<int, int> nav_id_count;//to be used by the server
	int navigation_count;//to give the id to the new navigation command

	int collide_count;
	int hit_bullet_count;
	int cost_upgrade_count;
	int bullet_count;
	int client_fire;
	int server_fire;
	bool gameOver;
	bool isFiring;
	int isShipInMotion()//this function has to be used
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);

		return motion;
	}

	bool frame_rate_limiter();//function to maintain the frame rate of the user function
	double threshold_health;
	double threshold_ammo;
	double threshold_fuel;
	
	deque<Event> current_event;//current events
	deque<Event> passive_event;//lingering events
	bool getNextCurrentEvent(Event& ob);
	deque<Event> getPassiveEvent()
	{
		unique_lock<mutex> lk(mutx->event_mutex[ship_id]);
		return passive_event;
	}//returns the passive event queue 
	int passive_event_point;//pointer to getNextPassiveEvent..to tell which event to send
private:
	int seconds;//seconds lived

	
	int minutes;//minutes lived
	int killer_ship_id;//the ship that killed you if that is the case
	int killer_cannon_id;//the cannon that killed you if that is the case
	deque<int> killed_ships;//the list of all the ships killed by you..
	int score;
	string name;
	int current_event_point;//pointer to current_event
	deque<navigation> nav_data;
	deque<navigation> nav_data_temp;//to store the nav_data temorarily
	deque<navigation> nav_data_final;//data that will be sent
	
	static List<Greed::cannon> cannon_list;//cannon class is required
	Greed::abs_pos front_abs_pos;//topmost coordinates of the tip of the ship:: will be updated in update_tile_pos
	Greed::abs_pos rear_abs_pos;//endmost coordinates of the ship ==> will be updated in update_tile_pos
	//Map::abs_pos absolutePosition;//always stores the top left coordinate of the ship tile
	Direction dir;
	//position is defined by the pointing part of the ship..look for it


	int radius;//square radius
	int bullet_radius;//radius of the bullet...initially this value is equal to the radius of the ship
	double health; //to be changed
	double gold;// to be changed
	double resources;// to be changed
	int died;//1 for dead, 0 for alive //to be changed
	int motion; //1 for moving, 0 for stop
	int fuel;//fuel is used for moving the ship around, once its over the ship cant move
	int invisible;//to be changed
	int ammo; // to be updated
	int bullet_pointer;
	int xbow; // to be updated
	int shield;//if this is on, then no bullet can damage the ship...this is given for a limited time
	//data structure responsible for movement

	
	Greed::coords tile_pos_front;
	Greed::coords tile_pos_rear;

	
	deque<shipInfo> shipInfoList;

	//for armory
	//now some functions
private:
	void updateFuel(int n)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		fuel = n;
	}
	Greed::abs_pos getAbsolutePosition1(ShipSide s = ShipSide::FRONT)//this function is without the mutex
	{

		if (s == ShipSide::FRONT)
			return front_abs_pos;
		else if (s == ShipSide::REAR)
		{
			return rear_abs_pos;
		}


	}
	
public:
	
	Greed::abs_pos getRealAbsolutePosition()//returns the top left corner of the ship sprite
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return absolutePosition;
	}
	Greed::abs_pos getAbsolutePosition(ShipSide s = ShipSide::FRONT)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (s == ShipSide::FRONT)
			return front_abs_pos;
		else if (s == ShipSide::REAR)
		{
			return rear_abs_pos;
		}


	}
	
public:
	void Greed_updateCost(Greed::coords ob, double new_cost);//new function to update the stuff
	
	vector<int> cannonsInMyRadius();
	vector<int> shipsInMyRadius();
	vector<int> cannonsIamInRadiusOf();//this function returns the cannon id having ship in radius.
	vector<int> shipsIamInRadiusOf();//returns the list of the ships who are overshadowing the current ship


	vector<Greed::coords> getRadiusCoords_ship(int s_id);//function to return the tiles that are in the radius of a particular entity, just pass the id of the ship
	vector<Greed::coords> getRadiusCoords_cannon(int c_id);//function to return the tiles that are in the radius of the cannon
	ship();
	List<Greed::bullet>& getBulletHitList()
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return bullet_hit;
	}
	int getTotalBulletsHit()
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return bullet_hit.howMany();
	}
	/*
	int getTotalBulletsFired()
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return cannon_ob.allBullets.howMany();
	}
	*/

	Greed::bullet getLastHitBullet()//returns the last bullet that had hit the ship
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return bullet_hit[bullet_hit.howMany() - 1];
	}
	Greed::bullet getLastBulletFired()//returns the last fired bullet
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return cannon_ob.activeBullets[cannon_ob.activeBullets.size() - 1];
	}
	vector<Greed::bullet> getHitBulletList()//list of the bullets that had hit the ship
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		vector<Greed::bullet> ret;
		for (int i = 0; i < bullet_hit.howMany(); i++)
		{
			ret.push_back(bullet_hit[i]);
		}
		return ret;
	}
	
	//this class will have the API for  the user, some functions will be inaccessible to the user, inaccessible functions will be protected by an id
	///functions for returning things.
	deque<shipInfo> getShipList()
	{
		return shipInfoList;
	}

	int getShipRadius()
	{
		//unique_lock<mutex> lk(mutx->m[mutex_id]);
		return radius;
	}
	double getCurrentFuel()
	{
		unique_lock<mutex> lk(mutx->m[mutex_id]);
		return fuel;
	}
	Direction getShipDirection()
	{
		unique_lock<mutex> lk(mutx->m[mutex_id]);
		return dir;
	}
	int getShipId()
	{
		return ship_id;
	}
	Greed::coords getCurrentTile()//returns the front tile
	{
		unique_lock<mutex> lk(mutx->m[mutex_id]);
		return tile_pos_front;

	}
	Greed::coords getCurrenntTile_withoutMutex()
	{
		return tile_pos_front;
	}
	Greed::coords getCurrentRearTile_withoutMutex()
	{
		return tile_pos_rear;
	}
	Greed::coords getCurrentRearTile()//returns the rear tile of the ship
	{
		unique_lock<mutex> lk(mutx->m[mutex_id]);
		return tile_pos_rear;

	}

	double getcurrentRadius()
	{
		unique_lock<mutex> lk(mutx->m[mutex_id]);
		return radius;
	}
	double getCurrentHealth()
	{
		unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		return health;
	}

	int getDiedStatus()
	{
		unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		return died;
	}

	int getCurrentAmmo()
	{

		unique_lock<std::mutex> lk(mutx->m[mutex_id]);
		return ammo;
	}

	int getTotalShips()
	{
		return total;
	}
	double getCurrentGold()
	{
		unique_lock<mutex> lk(mutx->m[mutex_id]);
		return gold;
	}
	//functions for updating some values of the game

	//starting of the API
	attribute whatsHere(Greed::coords ob, int m = 0);//leave this for now
	// List<attribute> whatsHere(Map::abs_pos ob);

public:
	// bool updateCost(Greed::abs_pos ob,double new_cost);
	int count_upgrade;//number of times the agent has sent the upgrading request
	bool isCannonInRadius(int c_id, ShipSide side = ShipSide::FRONT);

	bool isInShipRadius(int s_id, Greed::coords ob, ShipSide side = ShipSide::FRONT);//check for a poitn
	bool isInShipRadius(int s_id, ShipSide myside = ShipSide::FRONT, ShipSide opponent_side = ShipSide::FRONT);//overloaded function for isInShipRadius
	//make a function which checks if a tile is in a ships radius or not
	bool isShipInMyRadius(int s_id, ShipSide myside = ShipSide::FRONT);//check for self

	// armory starts from here


	vector<Greed::cannon> getCannonList()
	{
		vector<Greed::cannon> ret;
		for (int i = 0; i < cannon_list.howMany(); i++)
		{
			ret.push_back(cannon_list[i]);
		}
		return ret;
	}
	
	int getIndex(List<Greed::coords> ob, Greed::coords val)
	{
		int index = -1;
		for (int i = 0; i < ob.howMany(); i++)
		{
			if (val.c == ob[i].c && val.r == ob[i].r)
			{
				index = i;
				return index;
			}
		}
		return index;

	}
	bool isShipMoving(int s_id)// tells if another player's ship is moving
	{
		ship* ob = shipInfoList[s_id].ob; //assuming that no ship entry is deleted and every ship is in increasing order of their ship_id
		if (ob->motion == 1)
		{
			return true;
		}
		else if (ob->motion == 0)
		{
			return false;
		}
	}
	bool isShipMoving()
	{
		//return isShipMoving(this->ship_id);
		 //current mechanism is to check if the list of absolute coords is >0
		unique_lock<mutex> lk(mutx->m[ship_id]);
		return motion;

	}
	

	double getDistance(int s_id);//returns the distance of s_id ship from the this->ship

	//introducing the new functions
	void Greed_sail(Direction d, int tiles = 1)
	{

		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (motion == 0)
		{
			navigation nav(1, Greed::coords(-1, -1), -1, tiles, d);
			nav_data.push_back(nav);
		}
	}
	void Greed_setPath(int s_id)
	{

		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (motion == 0)
		{
			navigation nav(0, Greed::coords(-1, -1), s_id, -1, Direction::NA);
			nav_data.push_back(nav);
		}
	}
	void Greed_setPath(Greed::coords ob)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (motion == 0)
		{
			navigation nav(0, ob, -1, -1, Direction::NA);
			nav_data.push_back(nav);
		}
	}
	void Greed_chaseShip(int s_id)
	{

		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (motion == 0)
		{
			navigation nav(2, Greed::coords(-1, -1), s_id, -1, Direction::NA);
			nav_data.push_back(nav);
			lock_chase_ship = 1;
		}

	}
	void Greed_anchorShip()
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (motion == 1)
		{
			navigation nav(3, Greed::coords(-1, -1), -1, -1, Direction::NA);
			nav_data.push_back(nav);
		}
	}
	//entity conversion functions
	void Greed_fireCannon(cannon can, int s_id, ShipSide s)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);

		bullet_info.push_back(bullet_data(0, can, s_id, s, -1));

	}
	void Greed_fireAtCannon(int c_id, cannon can = cannon::FRONT)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);

		bullet_info.push_back(bullet_data(1, can, -1, ShipSide::NA, c_id));
	}

	deque<upgrade_keeping> upgrade_queue;
	void Greed_upgradeHealth(int n)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (lock_health == 0)
		{

			udata.push_back(upgrade_data(1, n));
			lock_health = 1;


		}
	}
	void Greed_upgradeAmmo(int n)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (lock_ammo == 0)
		{

			udata.push_back(upgrade_data(0, n));
			lock_ammo = 1;

		}
	}
	void Greed_upgradeFuel(int n)
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		if (lock_fuel == 0)
		{

			udata.push_back(upgrade_data(2, n));
			lock_fuel = 1;

		}
	}

	
};

class cannon_data
{
public:
	int cid;
	double angle;
	int picture;//which picture to display
	double cannon_health;
	int is_cannon_dead;
	cannon_data()
	{

	}
	cannon_data(int c, double a, int pic, double h, int dead)
	{
		cid = c;
		angle = a;
		picture = pic;
		cannon_health = h;
		is_cannon_dead = dead;

	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//beginning to write the functions 

int shipInfo::getTotalShips()
{
	return ob->getTotalShips();
}
int shipInfo::getShipId()
{
	return ob->getShipId();
}
Greed::coords shipInfo::getCurrentTile_withoutMutex()
{
	return ob->getCurrenntTile_withoutMutex();
}
Greed::coords shipInfo::getCurrentRearTile_withoutMutex()
{
	return ob->getCurrentRearTile_withoutMutex();
}
int shipInfo::getShipRadius()
{
	return ob->getShipRadius();
}
double shipInfo::getCurrentHealth()
{
	return ob->getCurrentHealth();
}
double shipInfo::getCurrentGold()
{
	return ob->getCurrentGold();
}

double shipInfo::getCurrentFuel()
{
	return ob->getCurrentFuel();
}
int shipInfo::getDiedStatus()
{
	return ob->getDiedStatus();
}
Direction shipInfo::getShipDirection()
{
	return ob->getShipDirection();
}
Greed::coords shipInfo::getCurrentTile()
{
	return ob->getCurrentTile();
}
Greed::coords shipInfo::getCurrentRearTile()
{
	return ob->getCurrentRearTile();
}
Greed::abs_pos shipInfo::getRealAbsolutePosition()
{
	return ob->getRealAbsolutePosition();
}
//defining event class for all the events happening in the game
vector<Greed::coords> ship::getRadiusCoords_cannon(int c_id)
{
	if (cannon_list.howMany() > c_id && c_id >= 0)
	{


		vector<Greed::coords> arr;
		int radius = 2;
		Greed::coords position;

		position = cannon_list[c_id].getCannonPosition();


		int c1 = position.c - radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int c2 = position.c + radius;
		if (c1 < 0)
		{
			c1 = 0;
		}
		else if (c1 > columns)
		{
			c1 = columns - 1;
		}
		int r1 = position.r - radius;
		if (r1 < 0)
		{
			r1 = 0;
		}
		else if (r1 > rows)
		{
			r1 = rows - 1;
		}
		int r2 = position.r + radius;
		if (r2 < 0)
		{
			r2 = 0;
		}
		else if (r2 > rows)
		{
			r2 = rows - 1;
		}
		for (int i = r1; i <= r2; i++)
		{
			for (int j = c1; j <= c2; j++)
			{
				arr.push_back(Greed::coords(i, j));
			}
		}
		return arr;
	}

}
bool ship::frame_rate_limiter()
{
	if (!hector)
	{
		starting_time_limiter = chrono::steady_clock::now();
		hector = true;
	}
	chrono::steady_clock::time_point ending_time_limiter = chrono::steady_clock::now();
	chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(ending_time_limiter - starting_time_limiter);

	elapsed_time += time_span.count();
	int frame = (int)(elapsed_time * frame_rate_limit) % frame_rate_limit;
	if (elapsed_time > 1)
	{
		elapsed_time = 0;
	}
	if (frame != current_frame_no)
	{
		current_frame_no = frame;
		return true;
	}
	return false;
}

ship::ship()//default ctor for now
{
	this->collide_count = 0;
	this->hit_bullet_count = 0;
	this->navigation_count = 0;
	this->cost_upgrade_count = 0;
	this->count_upgrade = 0;
	bullet_count = 0;
	client_fire = 0;
	server_fire = 0;
	gameOver = false;
	hector = false;
	lock_ammo = 0;
	lock_fuel = 0;
	lock_health = 0;
	lock_chase_ship = 0;

	frame_rate_limit = 30;
	elapsed_time = 0;
	current_frame_no = -1;
	current_time = chrono::high_resolution_clock::time_point();

	isFiring = false;
	health = 200;
	motion = 0;
	autopilot = 0;

	//making the localMap here
	died = 0;
	tile_pos_front = Greed::coords(5, 5);
	dir = Direction::NORTH;
	pointPath = -1;
	pointTilePath = 0;
	fuel = DEF_FUEL;
	ammo = DEF_AMMO;
	radius = 2;
	bullet_radius = radius;
	current_event_point = 0;
	bullet_pointer = -1;
	shield = 0;
	gold = INITIAL_GOLD;
	killer_ship_id = -1;
	killer_cannon_id = -1;
	threshold_health = 10;
	threshold_ammo = 10;
	threshold_fuel = 10;
	score = 0;
	minutes = INT_MAX;
	seconds = INT_MAX;


}
vector<int> ship::cannonsIamInRadiusOf()// a new functions which returns all the cannons whose radius the ship is in;
{
	Control ob;
	vector<int> l;
	List<Greed::cannon> cannon_list = ob.getCannonList(2369);
	for (int i = 0; i < cannon_list.howMany(); i++)
	{
		if (cannon_list[i].isDead == true)
		{
			continue;
		}
		//checking first for the front side of the ship
		bool front = cannon_list[i].isShipInMyRadius(this->ship_id, ShipSide::FRONT);
		bool rear = cannon_list[i].isShipInMyRadius(this->ship_id, ShipSide::REAR);
		if (front || rear)
		{
			l.push_back(cannon_list[i].cannon_id);
		}
	}
	return l;
}
vector<Greed::coords> ship::getRadiusCoords_ship(int s_id)
{
	auto cmp = [](Greed::coords a, Greed::coords b) {
		if (a.r == b.r)
		{
			return a.c < b.c;
		}
		if (a.c == b.c)
		{
			return a.r < b.r;
		}
		else
		{
			return a.r < b.r;
		}
		};
	std::map<Greed::coords, int, decltype(cmp)> arr(cmp);
	Control obb;
	deque<ship*> l1;
	l1 = obb.getShipList(2369);

	int radius = l1[s_id]->getShipRadius();
	Greed::coords position;

	position = l1[s_id]->getCurrentTile();//first doing with the front position of the ship

	int c1 = position.c - radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int c2 = position.c + radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int r1 = position.r - radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	int r2 = position.r + radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	for (int i = r1; i <= r2; i++)
	{
		for (int j = c1; j <= c2; j++)
		{
			arr.insert(pair<Greed::coords, int>(Greed::coords(i, j), 1));
		}
	}
	//now doing the same stuff with rear coordinates

	position = l1[s_id]->getCurrentRearTile();

	c1 = position.c - radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	c2 = position.c + radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	r1 = position.r - radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	r2 = position.r + radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	for (int i = r1; i <= r2; i++)
	{
		for (int j = c1; j <= c2; j++)
		{
			arr.insert(pair<Greed::coords, int>(Greed::coords(i, j), 1));
		}
	}
	vector<Greed::coords> l;
	for (auto c : arr)
	{
		l.push_back(c.first);

	}
	return l;
}
vector<int> ship::shipsInMyRadius()//returns the list of ships currently in my radius
{
	//which side the user wants to check
	//here front and rear are of the calling ship
	vector<int> l;//the list to be returned
	for (int i = 0; i < shipInfoList.size(); i++)
	{
		if (shipInfoList[i].getShipId() != this->ship_id && shipInfoList[i].ob->died == 0)
		{
			bool front = isShipInMyRadius(shipInfoList[i].getShipId(), ShipSide::FRONT);
			bool rear = isShipInMyRadius(shipInfoList[i].getShipId(), ShipSide::REAR);
			if (front || rear)
			{
				l.push_back(shipInfoList[i].getShipId());
			}

		}

	}
	return l;
}

vector<int> ship::shipsIamInRadiusOf()
{
	vector<int> l;
	for (int i = 0; i < shipInfoList.size(); i++)
	{
		if (shipInfoList[i].getShipId() != this->ship_id && shipInfoList[i].ob->died == 0)
		{
			bool front, rear;//opponent's 
			//for FRONT side
			front = isInShipRadius(shipInfoList[i].getShipId(), getCurrentTile(), ShipSide::FRONT);
			rear = isInShipRadius(shipInfoList[i].getShipId(), getCurrentTile(), ShipSide::REAR);
			bool Front = false;
			bool Rear = false;
			if (front || rear)
			{

				Front = true;
			}

			//for Rear
			front = isInShipRadius(shipInfoList[i].getShipId(), getCurrentRearTile(), ShipSide::FRONT);
			rear = isInShipRadius(shipInfoList[i].getShipId(), getCurrentRearTile(), ShipSide::REAR);
			if (front || rear)
			{
				Rear = true;
			}
			if (Front || Rear)
			{
				l.push_back(shipInfoList[i].getShipId());
			}
		}
	}
	return l;
}

double ship::getDistance(int s_id)//returns the eucledian distance between two ships using the tile_pos
{
	//just like finding the heuristic this is not the exact distance:
	//this is just an approximation of the original distance, original distance might vary
	Greed::coords c1 = shipInfoList[s_id].getCurrentTile();//getting the position of the ship in question
	Greed::coords c2 = getCurrentTile();
	return sqrt(pow((c1.r - c2.r), 2) + pow((c1.c - c2.c), 2));

}
bool ship::getNextCurrentEvent(Event& ob)
{
	unique_lock<mutex> lk(mutx->event_mutex[ship_id]);
	if (current_event.size() > 0)
	{
		ob = current_event[0];
		current_event.pop_front();
		return true;
	}
	return false;



}
attribute ship::whatsHere(Greed::coords ob, int m)//pass the row and column of the tile to get whats there
{
	/*
	* if m is 0 then global map is looked into for the attribute information
	* if m is 1 then local map is looked into for attribute information
	*/
	// cout << "\n inside the function==>" << localGreed[10 * columns + 0].b.cost;
	int index = ob.r * columns + ob.c;//this is the index of ob in the graph
	List<Greed::vertex> graph;
	if (m == 0)
	{
		graph = getGlobalMapref(2369);
	}
	else if (m == 1)//call the localMap
	{
		//
		graph = localMap;

	}
	attribute att;
	Control cont;

	List<Greed::cannon> cannon = cont.getCannonList(2369);
	deque<ship*> sh = cont.getShipList(2369);
	att.entity = graph[index].b.tile_id;
	//checking for the water first if its a water then checking for the storm
	if (graph[index].b.tile_id == Entity::WATER)
	{


		att.cost = WATER_COST;


	}
	//checking if its land then it can be simple land,cannon, or bonus
	else if (graph[index].b.tile_id == Entity::LAND)
	{

		if (aux1(cannon, ob) != -1)
		{


			att.cannon_id = aux1(cannon, ob);
			att.cost = -1;

		}
		else
		{

			att.cost = -1;
		}
	}
	//checking for the ship
	int ind = -1;
	for (int i = 0; i < sh.size(); i++)
	{
		if (sh[i]->tile_pos_front == ob)
		{
			ind = i;
			break;
		}
	}
	if (ind != -1)
	{

		att.ship_id = ind;
	}
	if (m == 1)
	{
		att.cost = graph[index].b.cost;
	}
	return att;

}
void ship::Greed_updateCost(Greed::coords ob, double new_cost)
{
	if (new_cost > 0 && map_ob.isValidCoords(ob))
	{
		unique_lock<mutex> lk(mutx->m[ship_id]);
		map_cost_data.push_back(map_cost(new_cost, ob));
	}
}
bool ship::isInShipRadius(int s_id, Greed::coords ob, ShipSide opponent_side)//s_id is the id of the id whose radius has to be checked
{

	Control obb;
	deque<ship*> l1;
	l1 = obb.getShipList(2369);
	if (l1[s_id]->died == 1)
	{
		return false;
	}
	int radius = l1[s_id]->getShipRadius();
	Greed::coords position;
	if (opponent_side == ShipSide::FRONT)
	{
		position = l1[s_id]->getCurrenntTile_withoutMutex();
	}
	else if (opponent_side == ShipSide::REAR)
		position = l1[s_id]->getCurrentRearTile_withoutMutex();

	int c1 = position.c - radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int c2 = position.c + radius;
	if (c1 < 0)
	{
		c1 = 0;
	}
	else if (c1 > columns)
	{
		c1 = columns - 1;
	}
	int r1 = position.r - radius;
	if (r1 < 0)
	{
		r1 = 0;
	}
	else if (r1 > rows)
	{
		r1 = rows - 1;
	}
	int r2 = position.r + radius;
	if (r2 < 0)
	{
		r2 = 0;
	}
	else if (r2 > rows)
	{
		r2 = rows - 1;
	}
	//checking if its in the radius or not;
	if (ob.c >= c1 && ob.c <= c2 && ob.r >= r1 && ob.r <= r2)
	{
		return true;
	}
	else
		return false;
}
bool ship::isInShipRadius(int s_id, ShipSide myside, ShipSide opponent_side)
{
	if (shipInfoList[s_id].getDiedStatus() == 1)
	{
		return false;
	}
	for (int i = 0; i < shipInfoList.size(); i++)
	{
		if (shipInfoList[i].getShipId() != this->ship_id)
		{
			if (myside == ShipSide::FRONT)
			{
				return isInShipRadius(s_id, getCurrentTile(), opponent_side);
			}
			else if (myside == ShipSide::REAR)
			{
				return isInShipRadius(s_id, getCurrentRearTile(), opponent_side);
			}
		}
	}
}
bool ship::isCannonInRadius(int c_id, ShipSide side)
{
	if (cannon_list[c_id].isDead == true)
	{
		return false;
	}
	return isInShipRadius(this->ship_id, cannon_list[c_id].getCannonTile(), side);
}
vector<int> ship::cannonsInMyRadius()
{

	Control ob;
	List<Greed::cannon> cannon_list = ob.getCannonList(2369);
	vector<int> l;
	for (int i = 0; i < cannon_list.howMany(); i++)
	{
		if (cannon_list[i].isDead == true)
		{
			continue;
		}
		bool front = false;
		bool rear = false;
		front = isCannonInRadius(cannon_list[i].cannon_id, ShipSide::FRONT);
		rear = isCannonInRadius(cannon_list[i].cannon_id, ShipSide::REAR);
		if (front || rear)
		{
			l.push_back(cannon_list[i].cannon_id);

		}
	}
	return l;

}
bool ship::isShipInMyRadius(int s_id, ShipSide oppSide)//to check if s_id is in the callers ship radius
{
	//this works as follows
	/*
	* this checks if oppSide(side of the opponent) is in the radius of current ship irrespective of its cannon position(both for front and rear)
	*/
	Control ob;
	deque<ship*> l;
	l = ob.getShipList(2369);
	if (s_id <= l.size() - 1 && s_id >= 0 && l[s_id]->died == 0)
	{
		Greed::coords pos;
		//chekcing for front side of opponent first
		if (oppSide == ShipSide::FRONT)
		{
			Greed::coords pos = l[s_id]->getCurrenntTile_withoutMutex();
			bool check = isInShipRadius(this->ship_id, pos, ShipSide::FRONT);
			bool check1 = isInShipRadius(this->ship_id, pos, ShipSide::REAR);
			if (check || check1)
			{
				return true;
			}
			else
				return false;
		}
		else if (oppSide == ShipSide::REAR)
		{
			Greed::coords pos = l[s_id]->getCurrentRearTile_withoutMutex();
			bool check = isInShipRadius(this->ship_id, pos, ShipSide::FRONT);
			bool check1 = isInShipRadius(this->ship_id, pos, ShipSide::REAR);
			if (check || check1)
			{
				return true;
			}
			else
				return false;
		}



	}

}

