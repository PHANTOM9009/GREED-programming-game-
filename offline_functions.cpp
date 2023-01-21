#include "lib2.hpp"
void graphics::callable(Mutex* mutx, int code[rows][columns], Map& map_ob)//taking the ship object so as to access the list of the player
{
	deque<int> dying_ships;
	int ran = 0;
	this->mutx = mutx;
	long total_time = 0;
	//retriving the ship list from the control class
	Control nm;
	deque<ship*>& pl1 = nm.getShipList(2369);
	List<Greed::cannon>& cannon_list = nm.getCannonList(2369);//retiving the cannon list
	cout << "\n number of cannons are==>" << cannon_list.howMany();
	List<Greed::coords> opaque = nm.getOpaqueCoords();

	//will be removed just for testing purposes

	//sf::RenderWindow window(sf::VideoMode(1970, 1190), "graphics testing");
	tgui::Gui gui(window);

	window.setFramerateLimit(60);
	////////////////////////////////////////////////////////////////////////////////////////////////
	sf::Texture explosion_tex;
	explosion_tex.loadFromFile("map_1 attributes/explosion_set.png");//loading the animation for the explosion
	resizeTexture(explosion_tex);//resizing the textures

	sf::Sprite explosion_sprite;
	explosion_sprite.setTexture(explosion_tex);
	////////////////////////////////////////////////////////////////////////////////////////////////
	List<graphics::animator> animation_list;
	//sf::Sprite rect[2];
	sf::Texture tex_s;
	tex_s.loadFromFile("map_1 attributes/ship_s.png");
	resizeTexture(tex_s);
	cout << "\n size is==>" << tex_s.getSize().x << " " << tex_s.getSize().y;
	sf::Texture tex_n;//texture of north
	tex_n.loadFromFile("map_1 attributes/ship_n.png");
	resizeTexture(tex_n);
	sf::Texture tex_e;
	tex_e.loadFromFile("map_1 attributes/ship_e.png");
	resizeTexture(tex_e);
	sf::Texture tex_w;
	tex_w.loadFromFile("map_1 attributes/ship_w.png");
	resizeTexture(tex_w);
	sf::Texture tex_ne;
	tex_ne.loadFromFile("map_1 attributes/ship_ne.png");
	resizeTexture(tex_ne);
	sf::Texture tex_nw;
	tex_nw.loadFromFile("map_1 attributes/ship_nw.png");
	resizeTexture(tex_nw);
	sf::Texture tex_se;
	tex_se.loadFromFile("map_1 attributes/ship_se.png");
	resizeTexture(tex_se);
	sf::Texture tex_sw;
	tex_sw.loadFromFile("map_1 attributes/ship_sw.png");
	resizeTexture(tex_sw);
	sf::Texture cannon_tex;
	cannon_tex.loadFromFile("map_1 attributes/c0.png");//defualt cannon texture
	resizeTexture(cannon_tex);
	sf::Texture cannon_tex_pack;
	cannon_tex_pack.loadFromFile("map_1 attributes/cannon_set.png");
	resizeTexture(cannon_tex_pack);
	sf::Texture firing_tex;
	firing_tex.loadFromFile("map_1 attributes/firing.png");
	resizeTexture(firing_tex);

	for (int i = 0; i < cannon_list.howMany(); i++)//graphic initializer for the cannon of the map
	{
		cannon_list[i].graphic_initializer(cannon_tex, sf::Vector2f(::cx(cannon_list[i].tile.c * 80 + 40 + origin_x), ::cy(cannon_list[i].tile.r * 80 + 40 + origin_y)));
	}


	for (int i = 0; i < pl1.size(); i++)//graphic initializer for the ship of the user
	{
		pl1[i]->graphics_initializer(tex_n);
	}


	sf::Texture tex;
	map_ob.intialize_graphics(tex);//to set the textures on the map
	sf::RenderStates s;
	s.texture = &tex;
	tileMap ob;
	ob.setScene(columns, rows, origin_x, origin_y, tex, code);

	//working on player now

	sf::Clock clock;
	double elapsed_time = 0;
	int c = 0;
	/*
	sf::View view;
	view.reset(sf::FloatRect(0, 0, 1920, 1080));
	view.setCenter(400, 400);
	view.rotate(50);
	window.setView(view);
	*/
	double t2 = 0;
	int frames = 0;
	Greed::coords* prev = new Greed::coords[pl1.size()];
	for (int i = 0; i < pl1.size(); i++)//setting the previous position
	{
		prev[i] = pl1[i]->tile_pos_front;
	}
	total_time = 0;
	//handling the gui part from here
	//-----------------GUI----------------------------------
	tgui::Font font_bold("Fonts/bold.ttf");
	tgui::Font font1("Fonts/semiBoldItalic.ttf");
	sf::Font font2;
	font2.loadFromFile("Fonts/text.ttf");
	sf::Texture setting1;
	setting1.loadFromFile("map_1 attributes/settings_blue.png");
	resizeTexture(setting1);
	sf::Texture setting2;
	setting2.loadFromFile("map_1 attributes/settings_red.png");
	resizeTexture(setting2);
	sf::Texture health;
	health.loadFromFile("map_1 attributes/health.png");
	resizeTexture(health);
	sf::Texture fire;
	fire.loadFromFile("map_1 attributes/fire.png");
	resizeTexture(fire);
	sf::Texture gold;
	gold.loadFromFile("map_1 attributes/money1.png");
	resizeTexture(gold);
	sf::Texture fuel;
	fuel.loadFromFile("map_1 attributes/fuel1.png");
	resizeTexture(fuel);
	sf::Texture menu_texture;
	menu_texture.loadFromFile("map_1 attributes/menu.png");
	resizeTexture(menu_texture);
	sf::Texture close_button;
	close_button.loadFromFile("map_1 attributes/close-window.png");
	resizeTexture(close_button);
	GuiRenderer gui_renderer(pl1.size(), &window);
	tgui::Theme::setDefault("TransparentGrey.txt");
	auto group1 = tgui::Group::create();//group for the top stats view 
	auto group2 = tgui::Group::create();//group for the panel view of the stats
	//gui function call starts

	gui_renderer.sp_renderer(group1);
	gui_renderer.p_renderer();
	gui_renderer.player_name_renderer(font1);
	gui_renderer.score_renderer1();
	gui_renderer.fire_renderer1(fire);
	gui_renderer.health_renderer(health);
	gui_renderer.gold_renderer1(gold);
	gui_renderer.fuel_renderer1(fuel);
	gui_renderer.score_renderer2();

	gui_renderer.fire_value_renderer();
	gui_renderer.gold_value_renderer();

	gui_renderer.health_value_renderer();
	gui_renderer.fuel_value_renderer();
	gui_renderer.menu_button_renderer(menu_texture, gui);
	gui_renderer.closeButton_renderer(close_button);
	gui_renderer.stats_view_renderer();
	gui_renderer.stats_view_panel_renderer(group2);
	gui_renderer.name_id_renderer(gui, font1);
	gui_renderer.time_line_text_renderer(gui, font_bold);
	gui_renderer.gameOverRenderer(font2);
	gui_renderer.winner_renderer(font2);
	gui_renderer.timer_renderer(gui);
	gui_renderer.setting_button_renderer(setting1, setting2, gui);

	gui_renderer.health_of_cannon_renderer(gui, font_bold);
	gui_renderer.final_window_renderer(gui);
	gui_renderer.firing_renderer(firing_tex);





	//gui_renderer.childWindowRenderer(gui);
	//gui function calls ends
	for (int i = 0; i < pl1.size(); i++)
	{
		gui_renderer.player_name[i]->setText(pl1[i]->name + "-" + std::to_string(pl1[i]->ship_id));

	}
	//call stats_view_renderer
	gui_renderer.time_line_renderer(gui);

	gui.add(group2);
	gui.add(group1, "first group of widgets");
	//mover_tgui<tgui::Picture::Ptr> moveit;

	//moveit.moving(window,gui,gui_renderer.firing_icon[0]);
	//write a function to check if a number is even or odd
	deque<int> dead_ship;//id of the dead ships
	bool gameOver = false;
	bool display = true;
	vector<int> winner;//id's of the winner of the game, this is a vector in case of a tie
	int var = 0;
	deque<int> deColor;//to decolor the tiles after illuminating them
	deque<ship*> duplicate_pl1;//duplicate for pl1


	//additional code to display the name of every player in above his ship
	sf::Text name_of_ship;
	sf::Font daemon;
	daemon.loadFromFile("Fonts/text.ttf");
	name_of_ship.setFont(daemon);
	name_of_ship.setCharacterSize((::cx(30) + ::cy(30)) / 2);
	vector<int> collided_ships;

	while (window.isOpen())
	{
		c++;
		frames++;
		sf::Event event;
		sf::Time deltime = clock.restart();
		elapsed_time += deltime.asSeconds();
		total_secs += deltime.asSeconds();
		//total_time += deltime.asSeconds();
		total_time++;
		t2 += deltime.asSeconds();
		if (t2 >= 1 && t2 <= 1.15)
		{
			cout << "\n frame rate is==>" << c;
			c = 0;
			t2 = 0;
		}
		while (window.pollEvent(event))
		{
			gui.handleEvent(event);
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))

			{
				window.close();

			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
			{
				window.setSize(sf::Vector2u(1920, 1080));
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
			{
				window.setSize(sf::Vector2u(1970, 1190));
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}

			if (event.type == sf::Event::Resized)
			{
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

			}


		}

		//setting the timer here


		if (total_secs / 60 >= 10 && total_secs / 60 <= 10.15)//game is run for 10 minutes
		{
			duplicate_pl1 = findWinner(pl1);
			gameOver = true;
		}
		if (check_game_over(pl1))//when alive ships are more than 1
		{
			//the game should be over now...
			gameOver = true;
			duplicate_pl1 = findWinner(pl1);
		}
		//illuminating the panel view whenever a mouse gets on the ship
		//setting the health for cannon
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

		if (!gameOver)
		{
			/*code to update the timer*/
			int min = total_secs / 60;
			int sec = (int)total_secs % 60;
			gui_renderer.timer->setText(std::to_string(min) + ":" + std::to_string(sec));
			/*code to update the timer ends*/
			for (int i = 0; i < pl1.size(); i++)
			{
				//getPointPath has to be protected by a mutex
				if (pl1[i]->died == 1)
				{
					continue;
				}
				if (pl1[i]->isFiring)
				{
					gui_renderer.firing_icon[i]->setVisible(true);
				}
				if (c % 17 == 0)
				{

					if (!pl1[i]->isFiring)
					{
						gui_renderer.firing_icon[i]->setVisible(false);
					}
				}
				if (mutx->m[pl1[i]->getMutexId(2369)].try_lock())
				{
					/*code to detect the collision
					*/
					for (int j = 0; j < pl1.size(); j++)
					{
						if (i != j && pl1[j]->died == 0)
						{

							if (pl1[i]->collide(j, pl1[i]->tile_pos_front))
							{
								//adding in queue of 

								pl1[i]->collided_ships.clear();
								pl1[i]->collided_ships.push_back(j);

								mutx->m[i].unlock();
								pl1[i]->anchorShip_collision();
								mutx->m[i].lock();
								//pl1[j]->anchorShip_collision();
								//cout << "\n location of ship2==>" << pl1[1]->absolutePosition.x+length << " " << pl1[1]->absolutePosition.y+length;
							}
						}
					}
					/*code to detect the collision ends*/
					List<Greed::abs_pos>& l1 = pl1[i]->getPathList(2369);

					if (pl1[i]->fuel > 0 && l1.howMany() > 0 && l1.howMany() - 1 != pl1[i]->getPointPath(2369))
					{
						// cout << "\n fuel " << i << " now=>" << pl1[i]->fuel;
						pl1[i]->motion = 1;

						pl1[i]->update_pointPath(pl1[i]->getPointPath(2369) + 1, 2369);

						pl1[i]->rect.setPosition(::cx(l1[pl1[i]->getPointPath(2369)].x + origin_x), ::cy(l1[pl1[i]->getPointPath(2369)].y + origin_y));

						pl1[i]->update_tile_pos(l1[pl1[i]->getPointPath(2369)].x, l1[pl1[i]->getPointPath(2369)].y);

						if (prev[i] != pl1[i]->tile_pos_front)//updating the fuel here
						{
							prev[i] = pl1[i]->tile_pos_front;
							pl1[i]->fuel -= pl1[i]->globalMap[pl1[i]->tile_pos_front.r * columns + pl1[i]->tile_pos_front.c].b.cost;
							if (pl1[i]->fuel <= 0)
							{
								mutx->m[pl1[i]->mutex_id].unlock();
								pl1[i]->anchorShip();
								mutx->m[pl1[i]->mutex_id].lock();

							}

						}
						//pl1[i]->fuel -= pl1[i]->globalMap[pl1[i]->tile_pos_front.r * columns + pl1[i]->tile_pos_front.c].b.cost;
						/*
						if (pl1[i]->ship_id == 1)
						{
							cout << "\n position is==>" << pl1[1]->getShipEntity().c1.x << " " << pl1[1]->getShipEntity().c1.y;
						}
						*/



						//setting the appropriate sprite for the ship
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
						//cout << "\n rectangle position according to game==>" << rect1.getPosition().x - 17 << " " << rect1.getPosition().y - 19 << endl;
						//cout << "\n tile position is==>" << pl1.tile_pos.r << " " << pl1.tile_pos.c << endl;

					}
					else
					{
						pl1[i]->motion = 0;
					}
					//started the rendering of the bullets of the current ship's cannon
					if (pl1[i]->ammo > 0 && pl1[i]->cannon_ob.activeBullets.size() > 0 && frames % 30 == 0)
					{
						frames = 0;
						int val1 = (pl1[i]->cannon_ob.activeBullets.size() - 1);
						int val2 = pl1[i]->bullet_pointer;
						if (val1 > val2)
						{
							pl1[i]->bullet_pointer++;
							Greed::bullet& bull = pl1[i]->cannon_ob.activeBullets[pl1[i]->bullet_pointer];
							if (bull.target_ship != -1)
								pl1[i]->setBullet(bull, bull.can, bull.target_ship, bull.s);
							else if (bull.target_cannon != -1)
							{
								pl1[i]->setBullet_forCannon(bull);
							}
						}
						pl1[i]->isFiring = true;

					}
					else
					{
						pl1[i]->isFiring = false;
					}
					for (int j = 0; j <= pl1[i]->bullet_pointer; j++)
					{


						if (pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory.size() > 0)
						{
							pl1[i]->cannon_ob.activeBullets[j].absolute_position = Greed::abs_pos(pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory[0].x, pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory[0].y);
							pl1[i]->cannon_ob.activeBullets[j].bullet_entity.setPosition(sf::Vector2f(::cx(pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory[0].x + origin_x), ::cy(pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory[0].y + origin_y)));
							for (int l = 1; l <= 3; l++)//this is speed of bullet. skipping 3 pixels per frame, speed is 180 frames(pixels) per sec
							{
								if (pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory.size() > 0)
								{
									pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory.erase(pl1[i]->cannon_ob.activeBullets[j].bullet_trajectory.begin());
								}
							}
						}
						else //when bullet's trajectory is over and nothing happend
						{
							//pl1[i]->cannon_ob.allBullets[pl1[i]->cannon_ob.activeBullets[j].id].set_after_data(0, -1, false);
							auto it = pl1[i]->cannon_ob.activeBullets.begin();
							advance(it, j);
							pl1[i]->cannon_ob.activeBullets.erase(it);
							j--;
							pl1[i]->bullet_pointer--;
							continue;
						}
						if (j >= 0)
						{
							int found = 0;
							//checking if the bullet has hit an opaque tile in that case the bullet has to be deleted instantaniously
							for (int k = 0; k < opaque.howMany(); k++)
							{
								//   cout << "\n1";
								Greed::coords coord((int)(::ry(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().y) - origin_y) / len, (int)(::rx(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().x - origin_x)) / len);
								// cout << "\n2";

								if (coord == opaque[k] && !find(cannon_list, coord))
								{
									//pl1[i]->cannon_ob.allBullets[pl1[i]->cannon_ob.activeBullets[j].id].set_after_data(0, -1, false);
									auto it = pl1[i]->cannon_ob.activeBullets.begin();
									advance(it, j);
									pl1[i]->cannon_ob.activeBullets.erase(it);
									j--;
									found = 1;
									pl1[i]->bullet_pointer--;
									break;
								}


							}
							if (found == 1)
							{
								continue;
							}

							if (pl1[i]->cannon_ob.activeBullets.size() > 0)
							{

								//checking if bullet hit another ship
								Greed::coords cor = Greed::coords((int)(::ry(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().y) - origin_y) / 80, (int)(::rx(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().x) - origin_x) / 80);

								if (!pl1[i]->cannon_ob.activeBullets[j].hit_or_not)
								{


									for (int u = 0; u < pl1.size(); u++)
									{
										if (i != u && pl1[u]->died == 0)
										{
											if (pl1[i]->checkCollision(u, pl1[i]->cannon_ob.activeBullets[j]))
											{
												pl1[i]->cannon_ob.activeBullets[j].set_after_data(5, pl1[u]->ship_id, true);//the bullet is still active for animation
												pl1[i]->cannon_ob.activeBullets[j].hit_or_not = true;
												// cout << "\n collision taken";

												found = 1;
												break;
											}
										}
									}
									if (found == 1)
									{
										continue;
									}
									//checking if the bullet hit a cannon

									for (int k = 0; k < cannon_list.howMany(); k++)
									{
										if (cannon_list[k].isDead == true)
										{
											continue;
										}
										Greed::coords cor = Greed::coords((int)(::ry(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().y) - origin_y) / 80, (int)(::rx(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().x) - origin_x) / 80);
										if (cannon_list[k].tile == cor)
										{
											// pl1[i]->cannon_ob.allBullets[pl1[i]->cannon_ob.activeBullets[j].id].set_after_data_for_cannon(5, cannon_list[k].cannon_id, true);
											pl1[i]->cannon_ob.activeBullets[j].set_after_data_for_cannon(5, cannon_list[k].cannon_id, true);
											pl1[i]->cannon_ob.activeBullets[j].hit_or_not = true;

											found = 1;
											break;
										}
									}
									if (found == 1)
									{
										continue;
									}

								}


							}
							// cout << "\n end";

							if (pl1[i]->cannon_ob.activeBullets[j].hit_ship != -1)
							{
								//add an animation everytime a bullet gets deleted by colliding with a ship

									//pl1[i]->cannon_ob.allBullets[pl1[i]->cannon_ob.activeBullets[j].id].isActive = false;
									//add the bullet in the list of the hit bullet of the victim ship
								//increasing the score of the firing ship
								pl1[i]->score += 2;
								if (pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->health > 0)
								{
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->health -= pl1[i]->cannon_ob.activeBullets[j].power;
								}
								if (pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->gold >= GOLD_OFF_A_BULLET)
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->gold -= GOLD_OFF_A_BULLET;
								//
								//
								//here ships are killed like flies
								//

								if (pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->health <= 0)
								{
									//20% of money is taken from the victim ship
									dying_ships.push_back(pl1[i]->cannon_ob.activeBullets[j].hit_ship);
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->minutes = total_secs / 60;
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->seconds = (int)total_secs % 60;
									int ch = (30 * pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->gold) / 100;
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->gold -= ch;
									pl1[i]->gold += ch;
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->killer_ship_id = i;
									//passing the fuel of the victim ship
									pl1[i]->fuel += pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->fuel;
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->fuel = 0;
									//passing the ammo to the victim ship
									pl1[i]->ammo += pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->ammo;
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->ammo = 0;
									pl1[i]->killed_ships.push_back(pl1[i]->cannon_ob.activeBullets[j].hit_ship);
									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->died = 1;
									//gui_renderer.player_panels[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->getRenderer()->setBackgroundColor(sf::Color::Red);
									mutx->timeMutex[i].lock();
									timeline t;
									t.eventype = timeline::EventType::DEFEATED_SHIP;
									t.timestamp = total_secs;
									t.d.id = pl1[i]->cannon_ob.activeBullets[j].hit_ship;
									pl1[i]->time_line.push_back(t);
									mutx->timeMutex[i].unlock();
									timeline t1;
									t1.eventype = timeline::EventType::SHIP_DIED;
									t1.timestamp = total_secs;

									pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->time_line.push_back(t1);
									pl1[i]->score += 100;


								}
								pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->bullet_hit.add_rear(pl1[i]->cannon_ob.activeBullets[j]);
								//bullet hit tempo for finding the bullet_hit event
								pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->bullet_hit_tempo.push_back(pl1[i]->cannon_ob.activeBullets[j]);
								animation_list.add_rear(animator(sf::Vector2f(::rx(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().x), ::ry(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().y)), elapsed_time, ANIMATION_TYPE::EXPLOSION, sf::Vector2f(pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->absolutePosition.x + origin_x, pl1[pl1[i]->cannon_ob.activeBullets[j].hit_ship]->absolutePosition.y + origin_y), pl1[i]->cannon_ob.activeBullets[j].hit_ship));



								auto it = pl1[i]->cannon_ob.activeBullets.begin();
								advance(it, j);
								pl1[i]->cannon_ob.activeBullets.erase(it);
								j--;
								pl1[i]->bullet_pointer--;

							}
							if (j >= 0 && pl1[i]->cannon_ob.activeBullets[j].hit_cannon != -1 && pl1[i]->cannon_ob.activeBullets[j].ttl == 15)//this condition is specially for the cannon
							{
								/*
								* a ship always aims for the middle of the cannon, hence we wait for 15 frames for the animation
								* of explosion since the cannon is static there should be no problem
								*/
								//here the cannon will suffer a  loss
								if (cannon_list[pl1[i]->cannon_ob.activeBullets[j].hit_cannon].health >= pl1[i]->cannon_ob.activeBullets[j].power)
								{
									cannon_list[pl1[i]->cannon_ob.activeBullets[j].hit_cannon].health -= pl1[i]->cannon_ob.activeBullets[j].power;
									pl1[i]->score += 2;
								}
								else
								{
									cannon_list[pl1[i]->cannon_ob.activeBullets[j].hit_cannon].health = 0;

								}
								//if the cannon is dead..give the 1000 money to the killer ship, and set dead status in cannon list
								if (cannon_list[pl1[i]->cannon_ob.activeBullets[j].hit_cannon].health == 0)
								{
									cannon_list[pl1[i]->cannon_ob.activeBullets[j].hit_cannon].isDead = true;
									pl1[i]->score += 100;
									pl1[i]->gold += 1000;
								}
								//pl1[i]->cannon_ob.allBullets[pl1[i]->cannon_ob.activeBullets[j].id].isActive = false;
								animation_list.add_rear(animator(sf::Vector2f(::rx(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().x), ::ry(pl1[i]->cannon_ob.activeBullets[j].bullet_entity.getPosition().y)), elapsed_time, ANIMATION_TYPE::EXPLOSION));

								auto it = pl1[i]->cannon_ob.activeBullets.begin();
								advance(it, j);
								pl1[i]->cannon_ob.activeBullets.erase(it);
								j--;
								pl1[i]->bullet_pointer--;
							}
							else if (j >= 0 && pl1[i]->cannon_ob.activeBullets[j].hit_or_not)
							{
								pl1[i]->cannon_ob.activeBullets[j].ttl++;
							}
						}
					}
					if (pl1[i]->health <= 0)
					{
						pl1[i]->died = 1;
					}

					//setting the panel color to red of those shis who have died
					if (pl1[i]->died == 1)
					{
						gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Red);
						gui_renderer.player_panels[i]->getRenderer()->setOpacity(0.6);
					}
					mutx->m[pl1[i]->getMutexId(2369)].unlock();
					//now updating the stats

					//another method for finding collisions between ships and bullets and ships
					//first of all using the method provided by sfml for sprite intersection for ship collision 


					//cout << pl1[0]->rect.getPosition().x;
					/*now working for the collisions:
					* 1. collision between bullet and ships
					*  1.1 check for only those ships which are in the bounding entity of the bullet
					*
					* 2. collisiton between ships and other ships
					*
					*/
				}
			}
			//for rendering and updating the visual stats in the game


			//bullet working for the cannon
			for (int j = 0; j < cannon_list.howMany(); j++)
			{
				if (cannon_list[j].isDead == true)
				{
					continue;
				}
				for (int k = 0; k < cannon_list[j].bullet_list.size(); k++)
				{

					if (cannon_list[j].bullet_list[k].bullet_trajectory.size() > 0)
					{
						cannon_list[j].bullet_list[k].absolute_position = Greed::abs_pos(cannon_list[j].bullet_list[k].bullet_trajectory[0].x, cannon_list[j].bullet_list[k].bullet_trajectory[0].y);
						cannon_list[j].bullet_list[k].bullet_entity.setPosition(sf::Vector2f(cx(cannon_list[j].bullet_list[k].bullet_trajectory[0].x + origin_x), cy(cannon_list[j].bullet_list[k].bullet_trajectory[0].y + origin_y)));
						for (int l = 1; l <= 3; l++)
						{
							if (cannon_list[j].bullet_list[k].bullet_trajectory.size() > 0)
							{
								cannon_list[j].bullet_list[k].bullet_trajectory.erase(cannon_list[j].bullet_list[k].bullet_trajectory.begin());
							}
						}
					}
					else //when bullet's trajectory is over and nothing happend
					{
						//  cannon_list[j].allBullets[pl1[i]->cannon_ob.activeBullets[j].id].set_after_data(0, -1, false);
						cannon_list[j].bullet_list.erase(cannon_list[j].bullet_list.begin());
						k--;
						continue;
					}

					if (k >= 0)
					{

						if (cannon_list[j].bullet_list.size() > 0)
						{
							//cout << "\n reached";
							//ship vs ship collision

							// cout << "\n caught";
							// cout << "\n cor has==>" << cor.r << " " << cor.c;
							 //cout << "\n ship 0 has position==>" << pl1[0]->getCurrentTile().r << " " << pl1[0]->getCurrentTile().c;
							for (int u = 0; u < pl1.size(); u++)
							{

								if (checkCollision(u, cannon_list[j].bullet_list[k]))//collision is there
								{
									//cannon_list[j].allBullets[cannon_list[j].bullet_list[k].id].set_after_data(5, pl1[u]->ship_id, true);
									cannon_list[j].bullet_list[k].set_after_data(5, pl1[u]->ship_id, true);
									//pl1[i]->cannon_ob.activeBullets[j].set_after_data(5, pl1[u]->ship_id, true);//the bullet is still active for animation
									cannon_list[j].bullet_list[k].hit_or_not = true;
									// cout << "\n collision taken";
								}

							}
						}
						// cout << "\n end";
					}
					if (cannon_list[j].bullet_list[k].ttl == 1)//here cannon bullet strikes the ship
					{
						//add an animation everytime a bullet gets deleted by colliding with a ship
						cannon_list[j].bullet_list[k].isActive = false;
						//cannon_list[j].allBullets[cannon_list[j].bullet_list[k].id].isActive = false;

						//add the bullet in the list of the hit bullet of the victim ship
						if (pl1[cannon_list[j].bullet_list[k].hit_ship]->health >= cannon_list[j].bullet_list[k].power)
						{
							pl1[cannon_list[j].bullet_list[k].hit_ship]->health -= cannon_list[j].bullet_list[k].power;//reducing the power of the ship
						}
						else
						{
							pl1[cannon_list[j].bullet_list[k].hit_ship]->health = 0;
						}
						if (pl1[cannon_list[j].bullet_list[k].hit_ship]->health <= 0)
						{
							int id = cannon_list[j].bullet_list[k].hit_ship;
							pl1[id]->died = 1;


							pl1[id]->minutes = total_secs / 60;
							pl1[id]->seconds = (int)total_secs % 60;
							int ch = (30 * pl1[id]->gold) / 100;
							pl1[id]->gold -= ch;

							pl1[id]->killer_cannon_id = j;
							mutx->timeMutex[id].lock();
							timeline t1;
							t1.eventype = timeline::EventType::SHIP_DIED;
							t1.timestamp = total_secs;

							pl1[id]->time_line.push_back(t1);
							mutx->timeMutex[id].unlock();

						}

						pl1[cannon_list[j].bullet_list[k].hit_ship]->getBulletHitList().add_rear(cannon_list[j].bullet_list[k]);
						pl1[cannon_list[j].bullet_list[k].hit_ship]->bullet_hit_tempo.push_back(cannon_list[j].bullet_list[k]);

						animation_list.add_rear(animator(sf::Vector2f(rx(cannon_list[j].bullet_list[k].bullet_entity.getPosition().x), ry(cannon_list[j].bullet_list[k].bullet_entity.getPosition().y)), elapsed_time, ANIMATION_TYPE::EXPLOSION, sf::Vector2f((pl1[cannon_list[j].bullet_list[k].hit_ship]->absolutePosition.x + origin_x), pl1[cannon_list[j].bullet_list[k].hit_ship]->absolutePosition.y + origin_y), cannon_list[j].bullet_list[k].hit_ship));
						cannon_list[j].bullet_list.erase(cannon_list[j].bullet_list.begin());
						k--;
					}
					else if (cannon_list[j].bullet_list[k].hit_or_not)
					{
						cannon_list[j].bullet_list[k].ttl++;
					}

				}

			}
			//checking and deleting events from passive_event queue
			// /*
			// here we will delete all the events which are not happening
			//checking for event1
			for (int i = 0; i < pl1.size(); i++)
			{
				if (mutx->event_mutex[i].try_lock())
				{
					//deleting from the current_event

					pl1[i]->current_event_point = 0;
					for (int j = 0; j < pl1[i]->passive_event.size(); j++)
					{
						//checking for event1
						if (pl1[i]->passive_event[j].eventType == Event::EventType::ShipsInMyRadius)
						{
							//checking if this event is still happening, if not then delete this event
							bool check = false;
							vector<int> l = pl1[i]->shipsInMyRadius();
							if (l.size() == pl1[i]->passive_event[j].radiusShip.sid.size())
							{
								for (int k = 0; k < pl1[i]->passive_event[j].radiusShip.sid.size(); k++)
								{
									if (l[k] != pl1[i]->passive_event[j].radiusShip.sid[k])
									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[i]->passive_event.begin();
									advance(it, j);
									pl1[i]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}

						}
						//checking for next event
						//event2
						else if (pl1[i]->passive_event[j].eventType == Event::EventType::IamInShipRadius)
						{
							bool check = false;
							vector<int> l = pl1[i]->shipsIamInRadiusOf();
							if (l.size() == pl1[i]->passive_event[j].radiusShip.sid.size())
							{
								for (int k = 0; k < pl1[i]->passive_event[j].radiusShip.sid.size(); k++)
								{
									if (l[k] != pl1[i]->passive_event[j].radiusShip.sid[k])
									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[i]->passive_event.begin();
									advance(it, j);
									pl1[i]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}

						}
						//checking for event3
						else	if (pl1[i]->passive_event[j].eventType == Event::EventType::IamInCannonRadius)
						{
							bool check = false;
							vector<int> l = pl1[i]->cannonsIamInRadiusOf();
							if (l.size() == pl1[i]->passive_event[j].radiusCannon.cid.size())
							{


								for (int k = 0; k < pl1[i]->passive_event[j].radiusCannon.cid.size(); k++)
								{

									if (l[k] != pl1[i]->passive_event[j].radiusCannon.cid[k])

									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[i]->passive_event.begin();
									advance(it, j);
									pl1[i]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event4
						else if (pl1[i]->passive_event[j].eventType == Event::EventType::CannonsInMyRadius)
						{
							bool check = false;
							vector<int> l = pl1[i]->cannonsInMyRadius();
							if (l.size() == pl1[i]->passive_event[j].radiusCannon.cid.size())
							{
								for (int k = 0; k < pl1[i]->passive_event[j].radiusCannon.cid.size(); k++)
								{
									if (l[k] != pl1[i]->passive_event[j].radiusCannon.cid[k])
									{
										check = true;
										break;

									}
								}
								if (check == true)//event has to be deleted
								{
									auto it = pl1[i]->passive_event.begin();
									advance(it, j);
									pl1[i]->passive_event.erase(it);
									j--;
								}
							}
							else
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event7
						else if (pl1[i]->passive_event[j].eventType == Event::EventType::LowHealth)
						{

							if (pl1[i]->getCurrentHealth() > pl1[i]->threshold_health)
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event8 
						else if (pl1[i]->passive_event[j].eventType == Event::EventType::NoAmmo)
						{
							if (pl1[i]->getCurrentAmmo() > 0)
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event9
						else if (pl1[i]->passive_event[j].eventType == Event::EventType::LowAmmo)
						{
							if (pl1[i]->getCurrentAmmo() > pl1[i]->threshold_ammo)

							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event10

						else if (pl1[i]->passive_event[j].eventType == Event::EventType::NoFuel)
						{
							if (pl1[i]->getCurrentFuel() > 0)
							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}
						//checking for event 11
						else if (pl1[i]->passive_event[j].eventType == Event::EventType::LowFuel)
						{
							if (pl1[i]->getCurrentFuel() > pl1[i]->threshold_fuel)

							{
								auto it = pl1[i]->passive_event.begin();
								advance(it, j);
								pl1[i]->passive_event.erase(it);
								j--;
							}
						}

					}
					mutx->event_mutex[i].unlock();

				}
			}
			//event handling starts here

			for (int i = 0; i < pl1.size(); i++)
			{
				//handling events for all the ships
				if (mutx->event_mutex[i].try_lock())
				{
					//important info: here ids are not given to the events, they will be given only after their validation

					//adding events in the queue of the respective ships..
					//checking for shipsInMyRadius event first
					vector<int> l1 = pl1[i]->shipsInMyRadius();
					Event e1;
					//make all the events first then check it with passive_event in a single loop
					if (l1.size() > 0)//it means that event has happened
					{
						e1.initialize(total_time, Event::EventType::ShipsInMyRadius, pl1[i]->ship_id);

						e1.radiusShip = Event::Radius_ship(l1);

						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							//chekcing for event1
							if (pl1[i]->passive_event[j].eventType == Event::EventType::ShipsInMyRadius && e1.eventType == Event::EventType::ShipsInMyRadius)
							{
								//cout << "\n in event0==>" << abs(pl1[i]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[i]->passive_event[j].ttl - e1.timestamp) <= 5 && pl1[i]->passive_event[j].radiusShip.sid.size() == e1.radiusShip.sid.size())//event occuring in intervals of three frames
								{
									//if these conditions are satisfied then only check equality operation between the events
									gotcha = 1;
									int found = 0;
									for (int k = 0; k < pl1[i]->passive_event[j].radiusShip.sid.size(); k++)
									{
										if (!(pl1[i]->passive_event[j].radiusShip.sid[k] == e1.radiusShip.sid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 0)//this means that events are same
									{
										pl1[i]->passive_event[j].ttl = e1.timestamp;//updating the last occuring of the event in the passive event queue
										break;
									}
									else if (found == 1)//this is a new event
									{
										e1.setEventId();
										pl1[i]->current_event.push_back(e1);//added the event in the current_event
										pl1[i]->passive_event.push_back(e1);//added the event in the passive_event
										break;
									}
								}


							}
						}
						if (gotcha == 0)
						{

							e1.setEventId();
							pl1[i]->current_event.push_back(e1);//added the event in the current_event
							pl1[i]->passive_event.push_back(e1);//added the event in the passive_event

						}
					}
					//checking for IamInShipRadius

					Event e2;
					vector<int> l2 = pl1[i]->shipsIamInRadiusOf();
					if (l2.size() > 0)
					{
						e2.initialize(total_time, Event::EventType::IamInShipRadius, pl1[i]->ship_id);
						e2.radiusShip = Event::Radius_ship(l2);
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::IamInShipRadius && e2.eventType == Event::EventType::IamInShipRadius)
							{
								//cout << "\n in event1==>" << abs(pl1[i]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[i]->passive_event[j].ttl - e2.timestamp) <= 5 && pl1[i]->passive_event[j].radiusShip.sid.size() == e2.radiusShip.sid.size())//event occuring in intervals of three frames
								{
									//if these conditions are satisfied then only check equality operation between the events
									gotcha = 1;
									int found = 0;
									for (int k = 0; k < pl1[i]->passive_event[j].radiusShip.sid.size(); k++)
									{
										if (!(pl1[i]->passive_event[j].radiusShip.sid[k] == e2.radiusShip.sid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 0)//this means that events are same
									{
										//cout << "\n i worked";
										pl1[i]->passive_event[j].ttl = e2.timestamp;//updating the last occuring of the event in the passive event queue
										break;
									}
									else if (found == 1)//this is a new event
									{
										e2.setEventId();
										pl1[i]->current_event.push_back(e2);//added the event in the current_event
										pl1[i]->passive_event.push_back(e2);//added the event in the passive_event
										break;
									}
								}
								else if (e2.eventType == Event::EventType::IamInShipRadius)
								{
									e2.setEventId();
									pl1[i]->current_event.push_back(e2);//added the event in the current_event
									pl1[i]->passive_event.push_back(e2);//added the event in the passive_event
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e2.setEventId();
							pl1[i]->current_event.push_back(e2);//added the event in the current_event
							pl1[i]->passive_event.push_back(e2);//added the event in the passive_event
						}

					}
					//making for the event IamInCannonRadius
					Event e3;
					vector<int> l3 = pl1[i]->cannonsIamInRadiusOf();
					if (l3.size() > 0)
					{
						e3.initialize(total_time, Event::EventType::IamInCannonRadius, pl1[i]->ship_id);

						e3.radiusCannon = Event::Radius_cannon(l3);
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::IamInCannonRadius && e3.eventType == Event::EventType::IamInCannonRadius)
							{
								//cout << "\n in event2==>" << abs(pl1[i]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[i]->passive_event[j].ttl - e3.timestamp) <= 3 && pl1[i]->passive_event[j].radiusCannon.cid.size() == e3.radiusCannon.cid.size())
								{
									gotcha = 1;
									int found = 0;
									for (int k = 0; k < pl1[i]->passive_event[j].radiusCannon.cid.size(); k++)
									{
										if (!(pl1[i]->passive_event[j].radiusCannon.cid[k] == e3.radiusCannon.cid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 1)//the events are not equal this is a new event
									{
										e3.setEventId();
										pl1[i]->current_event.push_back(e3);
										pl1[i]->passive_event.push_back(e3);
										break;

									}
									else if (found == 0)//event is same
									{
										pl1[i]->passive_event[j].ttl = e3.timestamp;
										break;
									}
								}
								else if (e3.eventType == Event::EventType::IamInCannonRadius)
								{
									e3.setEventId();
									pl1[i]->current_event.push_back(e3);
									pl1[i]->passive_event.push_back(e3);
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e3.setEventId();
							pl1[i]->current_event.push_back(e3);
							pl1[i]->passive_event.push_back(e3);
						}

					}
					//making of the event CannonsInMyRadius
					Event e4;
					vector<int> l4 = pl1[i]->cannonsInMyRadius();
					if (l4.size() > 0)
					{
						e4.initialize(total_time, Event::EventType::CannonsInMyRadius, pl1[i]->ship_id);
						e4.radiusCannon = Event::Radius_cannon(l4);
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::CannonsInMyRadius && e4.eventType == Event::EventType::CannonsInMyRadius)
							{
								//cout << "\n in event3==>" << abs(pl1[i]->passive_event[j].ttl - e1.timestamp);
								if (abs(pl1[i]->passive_event[j].ttl - e4.timestamp) <= 3 && pl1[i]->passive_event[j].radiusCannon.cid.size() == e4.radiusCannon.cid.size())
								{
									int found = 0;
									gotcha = 1;
									for (int k = 0; k < pl1[i]->passive_event[j].radiusCannon.cid.size(); k++)
									{
										if (!(pl1[i]->passive_event[j].radiusCannon.cid[k] == e4.radiusCannon.cid[k]))
										{
											found = 1;
											break;
										}
									}
									if (found == 1)//the events are not equal this is a new event
									{
										e4.setEventId();
										pl1[i]->current_event.push_back(e4);
										pl1[i]->passive_event.push_back(e4);
										break;

									}
									else if (found == 0)//event is same
									{
										pl1[i]->passive_event[j].ttl = e4.timestamp;
										break;
									}
								}
								else if (e4.eventType == Event::EventType::CannonsInMyRadius)
								{
									e4.setEventId();
									pl1[i]->current_event.push_back(e4);
									pl1[i]->passive_event.push_back(e4);
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e4.setEventId();
							pl1[i]->current_event.push_back(e4);
							pl1[i]->passive_event.push_back(e4);
						}
					}
					//making of the event ShipFire

					Event e5;
					unordered_map<int, Greed::bullet> l5;
					for (int j = 0; j < pl1[i]->bullet_hit_tempo.size(); j++)
					{
						if (pl1[i]->bullet_hit_tempo[j].launch_ship != -1)
						{

							l5.insert(pair<int, Greed::bullet>(pl1[i]->bullet_hit_tempo[j].launch_ship, pl1[i]->bullet_hit_tempo[j]));
							auto it = pl1[i]->bullet_hit_tempo.begin();
							advance(it, j);
							pl1[i]->bullet_hit_tempo.erase(it);
							j--;


						}
					}
					if (l5.size() > 0)//this event is set here
					{
						e5.initialize(total_time, Event::EventType::ShipFire, pl1[i]->ship_id);
						e5.setEventId();
						e5.shipFire = Event::ShipFire(l5);
						pl1[i]->current_event.push_back(e5);
					}

					//making of the event cannonFire
					Event e6;
					unordered_map<int, Greed::bullet> l6;
					for (int j = 0; j < pl1[i]->bullet_hit_tempo.size(); j++)
					{
						if (pl1[i]->bullet_hit_tempo[j].launch_cannon != -1)
						{
							l6.insert(pair<int, Greed::bullet>(pl1[i]->bullet_hit_tempo[j].launch_cannon, pl1[i]->bullet_hit_tempo[j]));
							auto it = pl1[i]->bullet_hit_tempo.begin();
							advance(it, j);
							pl1[i]->bullet_hit_tempo.erase(it);
							j--;

						}

					}
					if (l6.size() > 0)
					{
						e6.initialize(total_time, Event::EventType::CannonFire, pl1[i]->ship_id);
						e6.setEventId();

						e6.cannonFire = Event::CannonFire(l6);
						pl1[i]->current_event.push_back(e6);
						//hey
					}
					//making of the event LowHealth
					Event e7;

					if (pl1[i]->getCurrentHealth() < pl1[i]->threshold_health)//there will be a health threshold that can be set by every player
					{
						e7.initialize(total_time, Event::EventType::LowHealth, pl1[i]->ship_id);
						e7.lowHealth = Event::LowHealth(pl1[i]->getCurrentHealth());
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::LowHealth && e7.eventType == Event::EventType::LowHealth)
							{
								gotcha = 1;
								if (abs(pl1[i]->passive_event[j].ttl - e7.timestamp) <= 3)//the event need not be repeated
								{
									pl1[i]->passive_event[j].ttl = e7.timestamp;//updating the timestamp
									pl1[i]->passive_event[j].lowHealth.health = e7.lowHealth.health;//updating the new health
								}
								else
								{
									e7.setEventId();
									pl1[i]->passive_event.push_back(e7);
									pl1[i]->current_event.push_back(e7);
									break;


								}
							}
						}
						if (gotcha == 0)
						{
							e7.setEventId();
							pl1[i]->passive_event.push_back(e7);
							pl1[i]->current_event.push_back(e7);
						}
					}
					//making of event NoAmmo
					Event e8;
					if (pl1[i]->getCurrentAmmo() <= 0)
					{
						e8.initialize(total_time, Event::EventType::NoAmmo, pl1[i]->ship_id);
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::NoAmmo)
							{
								if (abs(pl1[i]->passive_event[j].ttl - e8.timestamp) <= 5)
								{
									pl1[i]->passive_event[j].ttl = e8.timestamp;
									gotcha = 1;
									break;
								}
							}
						}
						if (gotcha == 0)//this is a new event
						{
							e8.setEventId();
							pl1[i]->passive_event.push_back(e8);
							pl1[i]->current_event.push_back(e8);

						}

					}
					//making of the event LowAmmo
					Event e9;
					if (pl1[i]->getCurrentAmmo() < pl1[i]->threshold_ammo)
					{
						e9.initialize(total_time, Event::EventType::LowAmmo, pl1[i]->ship_id);
						e9.lowAmmo = Event::LowAmmo(pl1[i]->getCurrentAmmo());
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::LowAmmo)

							{
								if (abs(pl1[i]->passive_event[j].ttl - e9.timestamp) <= 5)
								{
									gotcha = 1;
									pl1[i]->passive_event[j].ttl = e9.timestamp;
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e9.setEventId();
							pl1[i]->passive_event.push_back(e9);
							pl1[i]->current_event.push_back(e9);
						}
					}

					//making of the event NoFuel
					Event e10;
					if (pl1[i]->getCurrentFuel() <= 0)
					{
						e10.initialize(total_time, Event::EventType::NoFuel, pl1[i]->getShipId());
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::NoFuel)

							{
								if (abs(pl1[i]->passive_event[j].ttl - e10.timestamp) <= 5)
								{
									gotcha = 1;
									pl1[i]->passive_event[j].ttl = e10.timestamp;
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e10.setEventId();
							pl1[i]->passive_event.push_back(e10);
							pl1[i]->current_event.push_back(e10);
						}

					}
					//making of the event LowFuel
					Event e11;
					if (pl1[i]->getCurrentFuel() < pl1[i]->threshold_fuel)
					{
						e11.initialize(total_time, Event::EventType::LowFuel, pl1[i]->getShipId());
						e11.lowFuel = Event::LowFuel(pl1[i]->getCurrentFuel());
						int gotcha = 0;
						for (int j = 0; j < pl1[i]->passive_event.size(); j++)
						{
							if (pl1[i]->passive_event[j].eventType == Event::EventType::LowFuel)

							{
								if (abs(pl1[i]->passive_event[j].ttl - e11.timestamp) <= 5)
								{
									gotcha = 1;
									pl1[i]->passive_event[j].ttl = e11.timestamp;
									break;
								}
							}
						}
						if (gotcha == 0)
						{
							e11.setEventId();
							pl1[i]->passive_event.push_back(e11);
							pl1[i]->current_event.push_back(e11);
						}
					}
					//now checking which ones of them are in passive_event queue and new events will be added in both the queues

					//adding the event of colliding of ships
					Event e12;

					if (pl1[i]->collided_ships.size() > 0)//if the event of collision has happend
					{
						e12.initialize(total_time, Event::EventType::ShipCollision, pl1[i]->ship_id);
						e12.setEventId();
						e12.shipCollision = Event::ShipCollision(pl1[i]->collided_ships);
						pl1[i]->current_event.push_back(e12);
						pl1[i]->collided_ships.clear();

					}




					mutx->event_mutex[i].unlock();
				}

			}
		}
		for (int i = 0; i < pl1.size(); i++)
		{
			string s = to_string((int)pl1[i]->getCurrentHealth());
			//updating the gui
			gui_renderer.score_value[i]->setText(to_string(pl1[i]->score));
			gui_renderer.health_value[i]->setText(s);
			gui_renderer.fire_value[i]->setText(to_string((int)pl1[i]->getCurrentAmmo()));
			gui_renderer.gold_value[i]->setText(to_string((int)pl1[i]->getCurrentGold()));
			gui_renderer.fuel_value[i]->setText(to_string((int)pl1[i]->getCurrentFuel()));
			if (pl1[i]->died == 1)
			{
				gui_renderer.player_panels[i]->getRenderer()->setBackgroundColor(sf::Color::Red);
				gui_renderer.player_panels[i]->getRenderer()->setOpacity(0.6);
			}
		}//for updating the upper boxes
		//to update the timeline listbox

		//writing code to implement the stats feature
		if (gui_renderer.m.try_lock())
		{
			if (gui_renderer.s_id != -1)
			{
				//cout << "\n panel opened for==>" << gui_renderer.s_id;
				string health_str = to_string(pl1[gui_renderer.s_id]->getCurrentHealth());
				const tgui::String h = static_cast<tgui::String>(health_str);
				gui_renderer.name_id->setText(pl1[gui_renderer.s_id]->name + "-" + to_string(pl1[gui_renderer.s_id]->ship_id));

				gui_renderer.stats_view->changeSubItem(0, 2, h);
				//updating gold value
				string gold_str = to_string(pl1[gui_renderer.s_id]->getCurrentGold());
				const tgui::String g = static_cast<tgui::String>(gold_str);
				gui_renderer.stats_view->changeSubItem(1, 2, gold_str);
				//updating Fuel value
				string fuel_str = to_string(pl1[gui_renderer.s_id]->getCurrentFuel());
				const tgui::String f = static_cast<tgui::String>(fuel_str);
				gui_renderer.stats_view->changeSubItem(2, 2, f);
				//updating ammo value
				string ammo_str = to_string(pl1[gui_renderer.s_id]->getCurrentAmmo());
				const tgui::String a = static_cast<tgui::String>(ammo_str);
				gui_renderer.stats_view->changeSubItem(3, 2, a);
				//updating tile coords(front)
				string tcfr = to_string(pl1[gui_renderer.s_id]->getCurrentTile().r);
				string tcfc = to_string(pl1[gui_renderer.s_id]->getCurrentTile().c);
				string fin = "Row: " + tcfr + " " + "Col.: " + tcfc;
				const tgui::String tcf = static_cast<tgui::String>(fin);
				gui_renderer.stats_view->changeSubItem(4, 2, tcf);
				//updating tile coords(back)
				string tcbfr = to_string(pl1[gui_renderer.s_id]->getCurrentRearTile().r);
				string tcbfc = to_string(pl1[gui_renderer.s_id]->getCurrentRearTile().c);
				string fin2 = "Row: " + tcbfr + " " + "Col.: " + tcbfc;
				const tgui::String tcb = static_cast<tgui::String>(fin2);
				gui_renderer.stats_view->changeSubItem(5, 2, tcb);
				//updating abs pos front
				string apfr = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::FRONT).x);
				string apfc = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::FRONT).y);
				string fin3 = "X: " + apfr + " " + "Y: " + apfc;
				const tgui::String apf = static_cast<tgui::String>(fin3);
				gui_renderer.stats_view->changeSubItem(6, 2, apf);
				//updating abs pos back
				string apbfr = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::REAR).x);
				string apbfc = to_string((int)pl1[gui_renderer.s_id]->getAbsolutePosition(ShipSide::REAR).y);
				string fin4 = "X: " + apbfr + " " + "Y: " + apbfc;
				const tgui::String apb = static_cast<tgui::String>(fin4);
				gui_renderer.stats_view->changeSubItem(7, 2, apb);
				//updating direction

				if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::NORTH)
				{
					const tgui::String dir = static_cast<tgui::String>("NORTH");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::EAST)
				{
					const tgui::String dir = static_cast<tgui::String>("EAST");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::WEST)
				{
					const tgui::String dir = static_cast<tgui::String>("WEST");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::SOUTH)
				{
					const tgui::String dir = static_cast<tgui::String>("SOUTH");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::NORTH_EAST)
				{
					const tgui::String dir = static_cast<tgui::String>("NORTH_EAST");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::NORTH_WEST)
				{
					const tgui::String dir = static_cast<tgui::String>("NORTH_WEST");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::SOUTH_EAST)
				{
					const tgui::String dir = static_cast<tgui::String>("SOUTH_EAST");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				else if (pl1[gui_renderer.s_id]->getShipDirection() == Direction::SOUTH_WEST)
				{
					const tgui::String dir = static_cast<tgui::String>("SOUTH_WEST");
					gui_renderer.stats_view->changeSubItem(8, 2, dir);
				}
				//updating the radius
				string rad = to_string(pl1[gui_renderer.s_id]->getShipRadius());
				const tgui::String r = static_cast<tgui::String>(rad);
				gui_renderer.stats_view->changeSubItem(9, 2, r);
				//updating cannon radius
				string crad = to_string(pl1[gui_renderer.s_id]->bullet_radius);
				const tgui::String cr = static_cast<tgui::String>(crad);
				gui_renderer.stats_view->changeSubItem(10, 2, cr);
				//updating cannnon power
				string cpow = to_string(pl1[gui_renderer.s_id]->cannon_ob.power);
				const tgui::String cpo = static_cast<tgui::String>(cpow);
				gui_renderer.stats_view->changeSubItem(11, 2, cpo);
				//updating the died status
				if (pl1[gui_renderer.s_id]->getDiedStatus() == 1)
				{
					const tgui::String str = "YES";
					gui_renderer.stats_view->changeSubItem(12, 2, str);
				}
				else if (pl1[gui_renderer.s_id]->getDiedStatus() == 0)
				{
					const tgui::String str = "NO";
					gui_renderer.stats_view->changeSubItem(12, 2, str);
				}
				//updating the shield status
				if (pl1[gui_renderer.s_id]->shield == 1)
				{
					const tgui::String str = "YES";
					gui_renderer.stats_view->changeSubItem(13, 2, str);
				}
				else if (pl1[gui_renderer.s_id]->shield == 0)
				{
					const tgui::String str = "NO";
					gui_renderer.stats_view->changeSubItem(13, 2, str);
				}
				//done updating the stats about the ship
				if (mutx->timeMutex[gui_renderer.s_id].try_lock())
				{

					int i = gui_renderer.s_id;
					if (pl1[i]->time_line.size() > 0)
					{
						if (pl1[i]->time_line[0].eventype == timeline::EventType::GOLD_TO_HEALTH)
						{
							vector<tgui::String> arr;
							arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
							string s = "Gold->Health. n=>";
							string s1 = to_string((int)pl1[i]->time_line[0].h.n) + " o=>" + to_string((int)pl1[i]->time_line[0].h.old);
							arr.push_back(static_cast<tgui::String>(s + s1));
							gui_renderer.timeline[i]->addItem(arr);
						}
						else if (pl1[i]->time_line[0].eventype == timeline::EventType::GOLD_TO_FUEL)
						{
							vector<tgui::String> arr;
							arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
							string s = "Gold->Fuel. n=>";
							string s1 = to_string((int)pl1[i]->time_line[0].f.n) + " o=>" + to_string((int)pl1[i]->time_line[0].f.old);
							arr.push_back(static_cast<tgui::String>(s + s1));
							gui_renderer.timeline[i]->addItem(arr);
						}
						else if (pl1[i]->time_line[0].eventype == timeline::EventType::GOLD_TO_AMMO)
						{
							vector<tgui::String> arr;
							arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
							string s = "Gold->Ammo. n=>";
							string s1 = to_string((int)pl1[i]->time_line[0].a.n) + " o=>" + to_string((int)pl1[i]->time_line[0].a.old);
							arr.push_back(static_cast<tgui::String>(s + s1));
							gui_renderer.timeline[i]->addItem(arr);
						}
						else if (pl1[i]->time_line[0].eventype == timeline::EventType::DEFEATED_SHIP)
						{
							vector<tgui::String> arr;
							arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
							string s = "Defeated ship=>" + pl1[pl1[i]->time_line[0].d.id]->name + "(" + to_string(pl1[pl1[i]->time_line[0].d.id]->ship_id) + ")";
							arr.push_back(s);
							gui_renderer.timeline[i]->addItem(arr);
						}
						else if (pl1[i]->time_line[0].eventype == timeline::EventType::ANCHOR_SHIP)
						{
							vector<tgui::String> arr;
							arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
							string s = "Ship is anchored";
							arr.push_back(s);
							gui_renderer.timeline[i]->addItem(arr);
						}
						else if (pl1[i]->time_line[0].eventype == timeline::EventType::SHIP_DIED)
						{
							vector<tgui::String> arr;
							arr.push_back(static_cast<tgui::String>(to_string(pl1[i]->time_line[0].timestamp)));
							string s = "Ship is defeated";
							arr.push_back(s);
							gui_renderer.timeline[i]->addItem(arr);
						}
						pl1[i]->time_line.pop_front();

					}
					mutx->timeMutex[i].unlock();
				}

			}
			gui_renderer.m.unlock();
		}//for updating the timeline boxes



		//drawing and animation part starts here
		//cout << "\n testing hello";
		window.clear();

		window.draw(ob.tile, s);
		for (int i = 0; i < pl1.size(); i++)
		{
			if (pl1[i]->died == 0)
			{
				window.draw(pl1[i]->rect);
				//drawing the names of the ships over them 
				name_of_ship.setPosition(::cx(pl1[i]->absolutePosition.x - 5 + origin_x), ::cy(pl1[i]->absolutePosition.y - 10 + origin_y));
				name_of_ship.setString(static_cast<sf::String>(pl1[i]->name));
				window.draw(name_of_ship);
			}

		}
		//cannon_list[0].fireCannon(1, ShipSide::FRONT);
		for (int i = 0; i < pl1.size(); i++)//for drawing the bullet entity
		{
			for (int j = 0; j <= pl1[i]->bullet_pointer; j++)
			{

				if (pl1[i]->died == 0)
				{
					pl1[i]->mutx->m[i].lock();
					window.draw(pl1[i]->cannon_ob.activeBullets[j].bullet_entity);
					pl1[i]->mutx->m[i].unlock();
				}
			}
		}
		//drawing the cannon sprite here

		for (int i = 0; i < cannon_list.howMany(); i++)
		{
			if (cannon_list[i].isDead == true)
			{
				continue;
			}
			//cannon_list[i].current_ship = 1;
			//cannon_list[i].ss = ShipSide::FRONT;
			int si = cannon_list[i].getVictimShip();
			double req_angle = cannon_list[i].get_required_angle();
			double current_angle = cannon_list[i].current_angle;
			/*
			cout << "\n current angle according to data==>" << current_angle;
			cout << "\n current angle according to sprtie==>" << cannon_list[i].cannon_sprite.getRotation();
			cout << "\n required angle==>" << req_angle;
			*/
			//cout << "\n  position of the cannon_sprite==>" << cannon_list[i].cannon_sprite.getGlobalBounds().left << " " << cannon_list[i].cannon_sprite.getGlobalBounds().top;

			if ((int)current_angle != (int)req_angle)
			{
				double diff = abs(req_angle - current_angle);

				if (diff < 5)
				{
					cannon_list[i].current_angle = req_angle;
					cannon_list[i].cannon_sprite.setRotation(req_angle);
					// cannon_list[i].fireCannon(1,ShipSide::FRONT);

				}


				else if (current_angle > req_angle)
				{
					cannon_list[i].cannon_sprite.rotate(-2.5);
					cannon_list[i].current_angle -= 2.5;
				}
				else if (current_angle < req_angle)
				{
					cannon_list[i].cannon_sprite.rotate(2.5);
					cannon_list[i].current_angle += 2.5;
				}

			}
			if ((int)req_angle == (int)current_angle)
			{

				if (c % 20 == 0)
				{
					int val = cannon_list[i].current_ship;
					if (val >= 0 && pl1[val]->died == 0 && (cannon_list[i].isShipInMyRadius(cannon_list[i].current_ship, ShipSide::FRONT)))
					{
						if (!gameOver)
						{
							cannon_list[i].fireCannon(cannon_list[i].current_ship);//this is the only place where we have to fire

							animation_list.add_rear(animator(sf::Vector2f(cannon_list[i].absolute_position.x + origin_x, cannon_list[i].absolute_position.y + origin_y), elapsed_time, ANIMATION_TYPE::CANNON_FIRE, cannon_list[i].cannon_id));
						}
					}
				}
			}

			window.draw(cannon_list[i].cannon_sprite);


		}
		//setting the base sprite on the cannon
		for (int i = 0; i < cannon_list.howMany(); i++)//setting the default texture of the cannon
		{
			if (cannon_list[i].isDead == true)
			{
				continue;
			}
			cannon_list[i].cannon_sprite.setTexture(cannon_tex);
		}



		for (int i = 0; i < cannon_list.howMany(); i++)
		{
			if (cannon_list[i].isDead == true)
			{
				continue;
			}
			// cout << "\n frame number==>" << c << " number of bullets to be printed==>" << cannon_list[i].bullet_list.howMany();
			for (int j = 0; j < cannon_list[i].bullet_list.size(); j++)
			{
				window.draw(cannon_list[i].bullet_list[j].bullet_entity);
			}
		}
		for (int i = 0; i < animation_list.howMany(); i++)
		{
			//cout << "\n second";
			   // cout << "\n bullet number==>" << i << " " << "ttl=>" << animation_list[i].ttl;
			if (animation_list[i].type == graphics::ANIMATION_TYPE::EXPLOSION)
			{
				if (animation_list[i].total_time <= animation_list[i].animation_time)
				{
					//here i am using moving animations i.e the frame is changing along with the position of the sprite
					if (animation_list[i].sid != -1)//this is for the ship
					{
						sf::Vector2f pos((animation_list[i].animation_offset.x + pl1[animation_list[i].sid]->absolutePosition.x + origin_x), (animation_list[i].animation_offset.y + pl1[animation_list[i].sid]->absolutePosition.y + origin_y));
						explosion_sprite.setPosition(sf::Vector2f(cx(pos.x), cy(pos.y)));
					}
					else
					{
						explosion_sprite.setPosition(sf::Vector2f(cx(animation_list[i].animation_position.x), cy(animation_list[i].animation_position.y)));
					}

					//cout << "\n--------------------------------";
					//cout << "\n for i==>" << i;
					animation_list[i].frame(elapsed_time, explosion_sprite);
					window.draw(explosion_sprite);
					//cout << "\n----------------------------------";
				}
				else
				{
					animation_list.delAt(i);
					i--;
				}
			}
			else if (animation_list[i].type == graphics::ANIMATION_TYPE::CANNON_FIRE)
			{


				if (animation_list[i].total_time <= animation_list[i].animation_time)
				{
					cannon_list[animation_list[i].cid].cannon_sprite.setTexture(cannon_tex_pack);
					animation_list[i].frame(elapsed_time, cannon_list[animation_list[i].cid].cannon_sprite);
				}
				else
				{

					animation_list.delAt(i);
					i--;
				}
			}


		}
		ob.setScene(columns, rows, origin_x, origin_y, tex, code);
		if (gui_renderer.showRadius)//to show radius of all the ships
		{

			for (int i = 0; i < pl1.size(); i++)
			{
				if (pl1[i]->getDiedStatus() == 0)
				{
					vector<Greed::coords> l = pl1[i]->getRadiusCoords_ship(i);
					for (int j = 0; j < l.size(); j++)
					{
						int ind = columns * l[j].r * 4 + 4 * l[j].c;
						ob.tile[ind].color = sf::Color::Green;
						ob.tile[ind + 2].color = sf::Color::Green;
						deColor.push_back(ind);

					}
				}
			}
		}

		if (gameOver && ran == 0)
		{

			gui_renderer.final_window->setVisible(true);
			for (int i = 0; i < pl1.size(); i++)
			{
				vector<tgui::String> data;
				data.push_back(static_cast<tgui::String>(i + 1));
				data.push_back(static_cast<tgui::String>(duplicate_pl1[i]->name));
				data.push_back(static_cast<tgui::String>(duplicate_pl1[i]->score));
				data.push_back(static_cast<tgui::String>(duplicate_pl1[i]->killed_ships.size()));
				if (duplicate_pl1[i]->minutes == INT_MAX && duplicate_pl1[i]->seconds == INT_MAX)
				{
					data.push_back("Not Defeated");
				}
				else
					data.push_back(static_cast<tgui::String>(to_string(duplicate_pl1[i]->minutes) + ":" + to_string(duplicate_pl1[i]->seconds)));
				gui_renderer.list1->addItem(data);
			}
			ran = 1;


		}
		if (gameOver)
		{
			window.draw(gui_renderer.gameOver);
		}
		gui.draw();


		window.display();

	}


}
int main()
{
	//extracting the data

	const int no_of_players = 7;
	cout << "\n number of players==>" << no_of_players;


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
	ship player[11];


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
	if (no_of_players == 1)//for 0 players
	{
		spawn.push_back(Greed::coords(10, 11));

	}
	else if (no_of_players == 2)
	{

		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));

	}
	else if (no_of_players == 3)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 11));
	}
	else if (no_of_players == 4)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 0));
		//spawn.push_back(Greed::coords(0,0));
		spawn.push_back(Greed::coords(10, 23));
	}
	else if (no_of_players == 5)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 23));
		spawn.push_back(Greed::coords(10, 11));
	}
	else if (no_of_players == 6)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 23));
		spawn.push_back(Greed::coords(10, 11));
		spawn.push_back(Greed::coords(2, 11));
	}
	else if (no_of_players == 7)
	{
		spawn.push_back(Greed::coords(1, 0));
		spawn.push_back(Greed::coords(1, 23));
		spawn.push_back(Greed::coords(6, 0));
		spawn.push_back(Greed::coords(6, 23));
		spawn.push_back(Greed::coords(10, 0));
		spawn.push_back(Greed::coords(10, 11));
		spawn.push_back(Greed::coords(10, 23));
	}
	for (int i = 0; i < no_of_players; i++)
	{
		if (i == 0)
			player[i].initialize_player("hawk", &mutx, silist, code, 5, spawn[i]);//last element is the tile pos of the player

		else if (i == 1)
		{
			player[i].initialize_player("dodger", &mutx, silist, code, 5, spawn[i]);
		}
		else if (i == 2)
		{
			player[i].initialize_player("normal", &mutx, silist, code, 5, spawn[i]);
		}
		else if (i == 3)
		{
			player[i].initialize_player("silent_killer", &mutx, silist, code, 5, spawn[i]);
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
	thread t[10];
	if (no_of_players == 1)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));

	}
	else if (no_of_players == 2)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
	}
	else if (no_of_players == 3)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
	}
	else if (no_of_players == 4)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
	}
	else if (no_of_players == 5)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
		t[4] = thread(user5::GreedMain, ref(player[4]));
	}
	else if (no_of_players == 6)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
		t[4] = thread(user5::GreedMain, ref(player[4]));
		t[5] = thread(user6::GreedMain, ref(player[5]));
	}
	else if (no_of_players == 7)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
		t[4] = thread(user5::GreedMain, ref(player[4]));
		t[5] = thread(user6::GreedMain, ref(player[5]));
		t[6] = thread(user7::GreedMain, ref(player[6]));
	}
	else if (no_of_players == 8)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
		t[4] = thread(user5::GreedMain, ref(player[4]));
		t[5] = thread(user6::GreedMain, ref(player[5]));
		t[6] = thread(user7::GreedMain, ref(player[6]));
		t[7] = thread(user8::GreedMain, ref(player[7]));
	}
	else if (no_of_players == 9)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
		t[4] = thread(user5::GreedMain, ref(player[4]));
		t[5] = thread(user6::GreedMain, ref(player[5]));
		t[6] = thread(user7::GreedMain, ref(player[6]));
		t[7] = thread(user8::GreedMain, ref(player[7]));
		t[8] = thread(user9::GreedMain, ref(player[8]));
	}
	else if (no_of_players == 10)
	{
		t[0] = thread(user1::GreedMain, ref(player[0]));
		t[1] = thread(user2::GreedMain, ref(player[1]));
		t[2] = thread(user3::GreedMain, ref(player[2]));
		t[3] = thread(user4::GreedMain, ref(player[3]));
		t[4] = thread(user5::GreedMain, ref(player[4]));
		t[5] = thread(user6::GreedMain, ref(player[5]));
		t[6] = thread(user7::GreedMain, ref(player[6]));
		t[7] = thread(user8::GreedMain, ref(player[7]));
		t[8] = thread(user9::GreedMain, ref(player[8]));
		t[9] = thread(user10::GreedMain, ref(player[9]));
	}
	for (int i = 0; i < no_of_players; i++)
	{
		t[i].detach();
	}





	graphics cg;
	cg.callable(&mutx, code, map1);


	char pBuf[256];
	size_t length = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, length);
	cout << "\n file path is==>" << pBuf;

	//code to extract the path of the directory in which the exe is
	string path;
	path.append(pBuf);
	for (int i = path.length() - 1; i >= 0; i--)
	{
		if (path[i] == '\\')
		{
			path.erase(i, path.length());
			break;
		}
	}
	cout << "\n directory is==>" << path;



}
