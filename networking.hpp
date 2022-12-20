
class pack_ship
{
private:
	int ship_id;//id of the ship


	int seconds;//seconds lived
	int minutes;//minutes lived
	int killer_ship_id;//the ship that killed you if that is the case
	int killer_cannon_id;//the cannon that killed you if that is the case
	int killed_ships_size;
	int killed_ships[1000];//the ships that you killed
	int score;
	string name;
	Greed::abs_pos front_abs_pos;//topmost coordinates of the tip of the ship:: will be updated in update_tile_pos
	Greed::abs_pos rear_abs_pos;//endmost coordinates of the ship ==> will be updated in update_tile_pos
	//Map::abs_pos absolutePosition;//always stores the top left coordinate of the ship tile
	Direction dir;
	//position is defined by the pointing part of the ship..look for it


	int radius;//square radius
	int bullet_radius;//radius of the bullet...initially this value is equal to the radius of the ship
	double health; //to be changed
	double gold;// to be changed
	int died;//1 for dead, 0 for alive //to be changed
	int motion; //1 for moving, 0 for stop
	int fuel;//fuel is used for moving the ship around, once its over the ship cant move
	int invisible;//to be changed
	int ammo; // to be updated
	int bullet_pointer;


	Greed::coords tile_pos_front;
	Greed::coords tile_pos_rear;
	Greed::abs_pos absolutePosition;
	bool isthere;//to tell if the time_line object contains anything

	timeline time_line;
public:
	pack_ship()
	{
		isthere = false;
	}
	pack_ship(int s_id, string name)
	{
		this->ship_id = s_id;
		this->name = name;

	}
	friend class graphics;
	friend void update_frame(deque<ship*>&, pack_ship&, int);
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
class animation_data
{
public:
	double x;
	double y;
	int picture;
};


class top_layer
{
public:
	double long total_secs;
	double long packet_no;//no. of the packet that is sent, this is used to keep track of the packet sent 
	//for the ships
	int no_of_players;
	pack_ship ob[10];
	//for the bullets
	int no_of_bullets;
	Greed::abs_pos bullet_pos[1000];
	//for cannons
	cannon_data cannon_ob[3];//object for cannon
	int no_of_animation;
	animation_data animation_ob[1000];


};