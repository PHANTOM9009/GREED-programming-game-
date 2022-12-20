/* code snippet for the client to display the health bar over the cannon
*/
int found = 0;
for (int i = 0; i < cannon_list.howMany(); i++)
{
	if (cannon_list[i].isDead == false)
	{
		if (cannon_list[i].cannon_sprite.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
		{

			gui_renderer.hoc->setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y - 20);
			gui_renderer.hoc->setText("Health: " + to_string((int)cannon_list[i].health));
			gui_renderer.hoc->setVisible(true);
			found = 1;

		}


	}
}
if (found == 0)
{
	gui_renderer.hoc->setVisible(false);
}
/*
* code to change the color of the panel of the user
* 
*/
for (int i = 0; i < pl1.size(); i++)
{
	if (pl1[i]->getDiedStatus() == 0)
	{
		if (pl1[i]->rect.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) == true)
		{
			gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color(0, 204, 122));//change the panel color to green
			//now illuminating the radius of the ship chosen


		}
		else
		{
			//gui_renderer.player_panels[i]->getRenderer()->setOpacity(1);
			gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Black);
			int s = deColor.size();

		}
	}
}
/*
* code to set the texture of the ship sprite
*/
if (pl1[i]->dir == Direction::NORTH)
{
	pl1[i]->rect.setTexture(tex_n);
}
else if (pl1[i]->dir == Direction::SOUTH)
{
	pl1[i]->rect.setTexture(tex_s);
}
else if (pl1[i]->dir == Direction::EAST)
{
	pl1[i]->rect.setTexture(tex_e);
}
else if (pl1[i]->dir == Direction::WEST)
{
	pl1[i]->rect.setTexture(tex_w);
}
else if (pl1[i]->dir == Direction::NORTH_EAST)
{
	pl1[i]->rect.setTexture(tex_ne);
}
else if (pl1[i]->dir == Direction::SOUTH_EAST)
{
	pl1[i]->rect.setTexture(tex_se);

}
else if (pl1[i]->dir == Direction::NORTH_WEST)
{
	pl1[i]->rect.setTexture(tex_nw);
}
else if (pl1[i]->dir == Direction::SOUTH_WEST)
{
	pl1[i]->rect.setTexture(tex_sw);
}
/*
* 
*/
//packets for clients
class pack_ship
{
public:
	int ship_id;//id of the ship
	int time_line_size;
	timeline time_line[1000];
	
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
	Greed::abs_pos absolutePosition;
};
//data frame for bullets
//the only thing needed for bullets is..its location
