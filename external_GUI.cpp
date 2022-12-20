#include<iostream>
#include "List.h"
#include<thread>
#include<mutex>
#include<math.h>
#include<string>
#include<SFML/Graphics.hpp>
#include<Windows.h>
#include<fstream>
#include<list>
#include<vector>
#include<iterator>
#include<map>
#include<algorithm>
#include<deque>
#include<unordered_map>
#include<windows.h>
#include<TGUI/Backend/SFML-Graphics.hpp>
#include<TGUI/TGUI.hpp>

//this will be present as an exe file, this code will be the ringmaster of the game
using namespace std;
int nop;//number of players
string path_to;//path to vcvars64
vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
sf::RenderWindow window(sf::VideoMode(modes[0].width, modes[0].height), "GREED");

int cx(int x)
{
	return ((x * modes[0].width) / 1920);
}
int cy(int y)
{
	return ((y * modes[0].height) / 1080);
}
class GuiRendererBase//default features that every gui widget will have
{
public:
	static void ButtonRenderer(tgui::Button::Ptr b)
	{
		
		
		b->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
	}
	static void LabelRenderer(tgui::Label::Ptr l)
	{
		l->setAutoSize(true);
		
		l->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
		l->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	}
	static void ScrollablePanelRenderer(tgui::ScrollablePanel::Ptr scrollable_panel)
	{

		scrollable_panel->getRenderer()->setRoundedBorderRadius(100);
		tgui::Scrollbar sc;
		sc.getRenderer()->setOpacity(0.5);
		scrollable_panel->getRenderer()->setScrollbar(sc.getRenderer()->getData());
		scrollable_panel->getRenderer()->setScrollbarWidth((::cx(13) + ::cy(13)) / 2);
	}
		
};
template<typename T>
class mover_tgui
{
public:
	void moving(sf::RenderWindow& window, tgui::Gui& gui, T button)
	{

		bool mouseClicked = false;
		bool dragging = false;
		sf::Vector2i mousepos;

		int x2, y2, x1, y1, X, Y;
		int m, n;
		int run = 0;
		int r = 0;
		int o = 0;

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();

				}

				if ((event.type == sf::Event::MouseButtonPressed) && (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
				{
					mouseClicked = true;
					//
					//cout<<mousepos.x<<" "<<mousepos.y<<endl;
					mousepos = sf::Mouse::getPosition(window);
					if ((button->getPosition().x <= mousepos.x && (button->getPosition().x + button->getSize().x) >= mousepos.x) && button->getPosition().y <= mousepos.y && (button->getPosition().y + button->getSize().y) >= mousepos.y)
					{
						dragging = true;
						x2 = event.mouseButton.x - button->getPosition().x;
						y2 = event.mouseButton.y - button->getPosition().y;

					}

				}

				if (event.type == sf::Event::MouseButtonReleased)
				{
					mouseClicked = false;
					dragging = false;
				}


				if (event.type == sf::Event::MouseMoved)
				{
					m = event.mouseMove.x;
					n = event.mouseMove.y;

				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					o = 1;
					cout << "\n the final position is==>" << button->getPosition().x << " " << button->getPosition().y;
					break;
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
				{

					cout << "\n middle";
				}

				gui.handleEvent(event); // Pass the event to the widgets
			}

			if (mouseClicked == true && dragging == true)
			{
				//cout<<m<<" "<<n<<endl;

				button->setPosition(sf::Mouse::getPosition(window).x - x2, sf::Mouse::getPosition(window).y - y2);
				//rect.setPosition(m-x2,n-y2);
				run++;
				r = 1;

			}
			else if (mouseClicked == false && dragging == false && run > 0 && r == 1)
			{
				cout << button->getPosition().x << " " << button->getPosition().y << endl;
				r = 0;
			}
			if (o == 1)
			{
				break;
			}
			window.clear(sf::Color::Cyan);
			//window.draw(button);
			gui.draw();
			window.display();

		}


	}


};
string getDirName(string filename)
{
	string name;
	for (int i = filename.length() - 1; i >= 0; i--)
	{
		if (filename[i] == '\\' || filename[i] == '/')
		{
			name = filename.substr(0, i + 1);
			break;
		}
	}
	return name;
}
string getFilename(string filename)
{
	string name;
	for (int i = filename.length() - 1; i >= 0; i--)
	{
		if (filename[i] == '\\' || filename[i] == '/')
		{
			name = filename.substr(i + 1, filename.length() - (i + 1));
			break;
		}
	}
	return name;
}
void putEscapeSeq(string& pb)//puts escape seq on the \, and double quotes at the beginning and the end of the stringf
{
	pb.push_back('\"');
	pb.insert(pb.begin(), '\"');
	int l = pb.length();
	for (int i = 0; i < l; i++)
	{
		if (pb[i] == '\\')
		{
			auto it = pb.begin();
			advance(it, i + 1);
			pb.insert(it, '\\');
			i++;
		}
	}
}
bool compile2()//code to compile the game.exe
{
	char pBuf[256];
	size_t length = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, length);
	cout << "\n file path is==>" << pBuf;
	string pb = pBuf;
	pb = getDirName(pb);
	putEscapeSeq(pb);
	string one = "cd " + pb;
	char en[1000];
	strcpy(en, pb.c_str());
	cout << "\n the new path is==>" << pb;

	string navigate = " && call " + path_to;
	string fin = one + navigate;
	string compile = " && cl /EHsc /MT -I\"final_libs\" /MD -I\"SFML visual c++\\SFML-2.5.1\\include\" -I\"TGUI-0.10-nightly\\include\" \"greed.cpp\" /link /LIBPATH:\"final_libs\" /LIBPATH:\"SFML visual c++\\SFML-2.5.1\\lib\" /LIBPATH:\"TGUI-0.10-nightly\\lib\"  lib2.lib tgui.lib sfml-graphics.lib sfml-audio.lib sfml-network.lib sfml-window.lib sfml-system.lib > final_logs.txt";
	string combine = fin + compile;
	char cmd1[1000];
	strcpy(cmd1, combine.c_str());
	cout << "\n the entire commmand is==>" << cmd1;
	int i=system(cmd1); 
	return i;
	
}
bool compileFile(string filename)
{
	//string navigate = "f: && cd \"F:\\GREED(programming game)\\GREED(programming game)\" && call \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64\"";
	char pBuf[256];
	size_t length = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, length);
	cout << "\n file path is==>" << pBuf;
	string pb = pBuf;
	pb=getDirName(pb);
	putEscapeSeq(pb);
	string one = "f: && cd " + pb;
	char en[1000];
	strcpy(en, pb.c_str());
	cout << "\n the new path is==>" << pb;
	
	string navigate = "call " + path_to;
	//string fin = one + navigate;
	filename.insert(filename.begin(), '\"');
	filename.append("\"");
	filename.insert(filename.begin(), ' ');
	filename.append(" ");
	string compile = " && cl /EHsc /MT -I\"user_libs\" /MD -I\"SFML visual c++\\SFML-2.5.1\\include\" -I\"TGUI-0.10-nightly\\include\"" +filename+ " /link /LIBPATH:\"user_libs\" /LIBPATH:\"SFML visual c++\\SFML-2.5.1\\lib\" /LIBPATH:\"TGUI-0.10-nightly\\lib\"  lib2.lib tgui.lib sfml-graphics.lib sfml-audio.lib sfml-network.lib sfml-window.lib sfml-system.lib > log.txt";

	//string compile = " && cl /MD -I\"user_libs\"" + filename + "/link /LIBPATH:\"user_libs\" lib2.lib > log.txt";
	string combine = navigate + compile;
	char cmd1[1000];
	strcpy(cmd1, combine.c_str());
	cout << "\n the entire command is==>" << cmd1;
	system(cmd1);
	//now openening the file
	ifstream in("log.txt", ios::in);
	char buff[100];
	while (in)
	{
		in >> buff;

		if (strcmp(buff, "LNK1561:") == 0)
		{
			in.close();
			return true;
		}

	}
	in.close();
	return false;
}
int current_number = 1;
string prepare(string filename)//function to remove the preprocessor statements from the user file and return the new name of the file
{
	//filename here is the full path of the file uploaded by the user
	
	ifstream in(filename, ios::in);
	string pseudo=getFilename(filename);//filename is the name of the file not the complete path
	
	char word[1000];
	vector<string> file;
	file.push_back("namespace user" + to_string(current_number) + "\n{");//adding a namespace to the file
	while (in)
	{
		in.getline(word,sizeof(word),'\n');
		file.push_back(word);
	}
	in.close();
	cout << "\n the entire file is==>";
	

	
	for (int i = 0; i < file.size(); i++)
	{
		for (int j = 0; j < file[i].length(); j++)
		{
			int st;
			if (file[i][j] == '#')
			{
				st = j;
				j++;
				
				while (j < file[i].length() && file[i][j] == ' ')//remove extra space
				{
					j++;
				}
				if (file[i].substr(j, 7) == "include")
				{
					j += 7;//at space
					
					while(j < file[i].length() && file[i][j] == ' ')
					{
						j++;
					}
					if (file[i][j] == '\"' || file[i][j] == '<')
					{
						j++;
						while (j < file[i].length() && file[i][j] == ' ')
						{
							j++;
						}
						if (file[i].substr(j, 12) == "user_lib.hpp")
						{
							j+=12;
							while (j < file[i].length() && file[i][j] == ' ')
							{
								j++;
							}
							if (file[i][j] == '\"' || file[i][j] == '>')
							{
								//delete the complete string here
								auto it = file[i].begin();
								advance(it, st);
								auto it1 = file[i].begin();
								advance(it1, j + 1);
								file[i].erase(it, it1);
							}
						}
					}
				}
				
			}
			
		}
	}
	int ind = -1;
	for (int i = 0; i < pseudo.length(); i++)
	{
		if (pseudo[i] == '.')
		{
			ind = i;
			break;
		}
	}
	file.push_back("\n}");
	//at this index insert the new string
	string temp = "_temp";
	pseudo.insert(ind, temp);
	pseudo = "user" + to_string(current_number) + "_temp.cpp";
	current_number++;
	ofstream out(pseudo, ios::out);
	for (int i = 0; i < file.size(); i++)
	{
		out << file[i]+"\n";
	}
	out.close();
	return pseudo;
}
class fileData//for data transfer between the two process
{
public:
	int no_of_players;
    string player_name;
	
	fileData(int no, string name)
	{
		no_of_players = no;
		player_name = name;
		
	}
};
class graphics
{
public:
	static int fin;//this is true when the game is ready to start
	class loadWidgets//loading all the widgets that are needed in the game
	{
	public:
		string path_to_compile;//path of the compiling vcvars64 of visual studio
		int no_of_players;
		int count;
		vector<string> player_name;//names of the user
		vector<string> algo_path;//paths of the user algorithms
		tgui::Button::Ptr offlinePlay;
		tgui::Button::Ptr onlinePlay;
		tgui::Group::Ptr window1;//play offline or online
		tgui::Group::Ptr window2;//how many players are playing
		tgui::Button::Ptr goBackWindow1;//to go back to window1 from window 2
		tgui::Label::Ptr l1;//to print how many players are playing?
		tgui::ComboBox::Ptr box1;//to select the number of players
		tgui::Button::Ptr button1;//to go to next window
		tgui::Group::Ptr window3;//upload the algorithms.
		tgui::Group::Ptr window4;
		tgui::Label::Ptr l2;
		tgui::FileDialog::Ptr file;
		tgui::Button::Ptr uploadFile;//button to upload the file
		tgui::EditBox::Ptr editbox;
		tgui::Button::Ptr button2;
		sf::Texture tex1;
		sf::Texture tex2;
		tgui::Label::Ptr wrong;//to display that the file has errors
		tgui::Button::Ptr start_game;
		tgui::Label::Ptr starting;//label to print that the game is starting
		
		tgui::Group::Ptr window0;//for uploading the path of visual studio for once
		tgui::Label::Ptr label0;
		tgui::Button::Ptr button0;//button for uploading the path of visual studio
		tgui::FileDialog::Ptr file0;//to upload the file
		tgui::Label::Ptr wrong1;//to print that the vcvars64 is wrong file
		
		
		loadWidgets()
		{
			count = 1;
			window1 = tgui::Group::create();
			window2 = tgui::Group::create();
			window3 = tgui::Group::create();
			window4 = tgui::Group::create();
			window0 = tgui::Group::create();
			
			window1->setVisible(false);
			window0->setVisible(false);
			window3->setVisible(false);
			window2->setVisible(false);
			window4->setVisible(false);
		}
		void raiseError_renderer()
		{
			wrong1 = tgui::Label::create();
			wrong1->setAutoSize(true);
			wrong1->getRenderer()->setBackgroundColor(sf::Color::Black);
			wrong1->getRenderer()->setTextColor(sf::Color::Red);
			wrong1->setText("This is not the desired file");
			wrong1->setTextSize((::cx(32) + ::cy(32)) / 2);
			wrong1->setPosition(::cx(580), ::cy(45));
			wrong1->showWithEffect(tgui::ShowEffectType::SlideToLeft, tgui::Duration(1000));
			//wrong1->setVisible(false);
			window0->add(wrong1);
		}
		void callback01(vector<tgui::Filesystem::Path> arr)
		{
			string str = arr[0].asString().toStdString();
			string check = "";
			for (int i = str.length() - 1; i >= 0; i--)
			{
				if (str[i] == '/'||str[i]=='\\')
				{
					check = str.substr(i + 1, str.length() - i);
					break;
				}
			}
			
			
			if (check == "vcvars64.bat")
			{
				path_to_compile = str;
				path_to = str;
				path_to.insert(path_to.begin(), '"');
				path_to.push_back('"');
				ofstream out("path.txt", ios::out);
				out << str;
				out.close();
				window0->setVisible(false);
				window1->setVisible(true);
			}
			else
			{
				raiseError_renderer();
			}
		}
		void file0Renderer()
		{
			file0 = tgui::FileDialog::create();
			file0->setSize(::cx(600), ::cy(600));
			file0->setPosition(::cx(674), ::cy(250));
			file0->getRenderer()->setBackgroundColor(sf::Color::Black);
			file0->onFileSelect(&graphics::loadWidgets::callback01, this);
			window0->add(file0);
		}
		void callback0()
		{
			file0Renderer();//to show the file renderer
		}
		void button0Renderer()
		{
			button0 = tgui::Button::create();

			GuiRendererBase::ButtonRenderer(button0);
			button0->setPosition(::cx(798), ::cy(428));
			button0->setSize(::cx(200), ::cy(100));
			button0->setText("Upload File");
			button0->setTextSize((::cx(32) + ::cy(32)) / 2);
			button0->getRenderer()->setBackgroundColor(sf::Color::Black);
			button0->getRenderer()->setTextColor(sf::Color::White);
			button0->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
			button0->getRenderer()->setRoundedBorderRadius(10);
			button0->getRenderer()->setOpacity(0.8);
			button0->onPress(&graphics::loadWidgets::callback0, this);
			window0->add(button0);
			
		}
		void label0Renderer()
		{
			label0 = tgui::Label::create();
			label0->setAutoSize(true);

			label0->setText("browse the path to vcvars64 file of visual studio: \n it is usually found at \n Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat");
			label0->setTextSize((::cx(28) + ::cy(28)) / 2);
			label0->setPosition(::cx(500), ::cy(184));
			label0 ->getRenderer()->setBackgroundColor(sf::Color::Black);
			label0->getRenderer()->setTextColor(sf::Color::White);
			window0->add(label0);
		}
		void wrongRenderer()
		{
			wrong = tgui::Label::create();
			wrong->setAutoSize(true);
			wrong->getRenderer()->setBackgroundColor(sf::Color::Black);
			wrong->getRenderer()->setTextColor(sf::Color::Red);
			wrong->setText("File has errors! solve the Errors and upload the file again!");
			wrong->setTextSize((::cx(32) + ::cy(32)) / 2);
			wrong->setPosition(::cx(580), ::cy(45));
			wrong->showWithEffect(tgui::ShowEffectType::SlideToLeft, tgui::Duration(1000));
			wrong->setVisible(false);
			window3->add(wrong);
			
		}
		void editBoxRenderer()
		{
			editbox = tgui::EditBox::create();
			editbox->setSize(::cx(500), ::cy(80));
			editbox->setPosition(::cx(669), ::cy(269));
			editbox->setDefaultText("Enter player name");
			editbox->setTextSize((::cx(22) + ::cy(22)) / 2);
			editbox->getRenderer()->setBackgroundColor(sf::Color::Black);
			editbox->getRenderer()->setTextColor(sf::Color::White);
			editbox->getRenderer()->setBackgroundColorHover(sf::Color(115,115,115));
			editbox->getRenderer()->setBackgroundColorFocused(sf::Color(115,115,155));
			editbox->setAlignment(tgui::EditBox::Alignment::Center);
			
			window3->add(editbox);

		}
		void callback6()//callback for button2
		{
			if (count == 1)
			{
				window3->setVisible(false);
				window2->setVisible(true);
			}
			else
			{
				count--;
				player_name.pop_back();
				algo_path.pop_back();
				reload_window3();
			}
			
		}
		void button2_renerer()//button in window3 to go back to subsequent windows
		{
			button2 = tgui::Button::create();
			button2->setSize(::cx(tex1.getSize().x), ::cy(tex1.getSize().y));
			button2->getRenderer()->setTexture(tex2);
			button2->getRenderer()->setTextureHover(tex1);
			button2->getRenderer()->setTextureDown(tex1);
			button2->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
			button2->setPosition(::cx(0), ::cy(0));
			button2->onPress(&graphics::loadWidgets::callback6,this);
			window3->add(button2);
		}
		void callback5(vector<tgui::Filesystem::Path> arr)
		{
				
				if (arr.size() > 0)
				{
					//here the file will be compiled: if compiled properly then the following code will execute, else an error message will appear
					cout << "\n compiling the file....";
					if (compileFile(arr[0].asString().toStdString()))//we are compiling the files of the users
					{
						wrong->setVisible(false);
						cout << "\n file compiled successfully";
						count++;
						if (count > no_of_players)
						{
							player_name.push_back(editbox->getText().toStdString());
							if (arr.size() > 0)
								algo_path.push_back(arr[0].asString().toStdString());
							//time for window4
							window3->setVisible(false);
							window4->setVisible(true);
						}
						else
						{
							player_name.push_back(editbox->getText().toStdString());
							if (arr.size() > 0)
								algo_path.push_back(arr[0].asString().toStdString());
							window3->setEnabled(false);
							//Sleep(500);
							reload_window3();
							cout << "\n reloaded the entire window";
						}
					}
					else
					{
						wrong->setVisible(true);
						cout << "\n the file has errors==>";
					}
				}
			
			else
			{
				cout << "\n please choose a file";
			}
			
		}
		void file_renderer()
		{
			file = tgui::FileDialog::create();
			file->setSize(::cx(600), ::cy(600));
			file->setPosition(::cx(674), ::cy(250));
			file->getRenderer()->setBackgroundColor(sf::Color::Black);
			file->onFileSelect(&graphics::loadWidgets::callback5, this);
			file->setFileTypeFilters({ {".txt or .cpp",{"*.cpp","*.txt"} } });
			window3->add(file);

		}
		void callback4()
		{
			file_renderer();
		}
		void uploadFile_renderer()
		{
			uploadFile = tgui::Button::create();
			GuiRendererBase::ButtonRenderer(uploadFile);
			uploadFile->setPosition(::cx(798), ::cy(428));
			uploadFile->setSize(::cx(200), ::cy(100));
			uploadFile->setText("Upload File");
			uploadFile->setTextSize((::cx(32) + ::cy(32)) / 2);
			uploadFile->getRenderer()->setBackgroundColor(sf::Color::Black);
			uploadFile->getRenderer()->setTextColor(sf::Color::White);
			uploadFile->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
			uploadFile->getRenderer()->setRoundedBorderRadius(10);
			uploadFile->getRenderer()->setOpacity(0.8);
			uploadFile->onPress(&graphics::loadWidgets::callback4, this);
			window3->add(uploadFile);
		}
		void starting_renderer()
		{
			starting = tgui::Label::create();
			starting->setAutoSize(true);
			
			starting->setText("Starting...");
			starting->setTextSize((::cx(40) + ::cy(40)) / 2);
			starting->setPosition(::cx(674), ::cy(184));
			starting->getRenderer()->setBackgroundColor(sf::Color::Black);
			starting->getRenderer()->setTextColor(sf::Color::White);
			window4->add(starting);
			
		}
		void l2_renderer()
		{
			l2 = tgui::Label::create();
			l2->setAutoSize(true);
			string st = "Upload algorithms: " + to_string(count) + "/" + to_string(no_of_players);
			l2->setText(st);
			l2->setTextSize((::cx(40) + ::cy(40)) / 2);
			l2->setPosition(::cx(674), ::cy(184));
			l2->getRenderer()->setBackgroundColor(sf::Color::Black);
			l2->getRenderer()->setTextColor(sf::Color::White);
			window3->add(l2);


		}
		void reload_window3()
		{
			window3->removeAllWidgets();
			
			l2_renderer();
			uploadFile_renderer();
			editBoxRenderer();
			button2_renerer();
			wrongRenderer();
			window3->setEnabled(true);
		}
		void callback2()//to go from window2 to window1
		{
			window1->setVisible(true);
			window2->setVisible(false);
		}
		void goBackWindow1Renderer()
		{
			goBackWindow1 = tgui::Button::create();
			goBackWindow1->setSize(::cx(tex1.getSize().x), ::cy(tex1.getSize().y));
			goBackWindow1->getRenderer()->setTexture(tex2);
			goBackWindow1->getRenderer()->setTextureHover(tex1);
			goBackWindow1->getRenderer()->setTextureDown(tex1);
			goBackWindow1->getRenderer()->setBorders(tgui::Borders::Outline(0, 0, 0, 0));
			
			goBackWindow1->setPosition(::cx(0), ::cy(0));
			goBackWindow1->onPress(&graphics::loadWidgets::callback2, this);
			window2->add(goBackWindow1);
			
		}
		void callback3()
		{
			window2->setVisible(false);
			window3->setVisible(true);
			no_of_players = stoi(box1->getSelectedItem().toStdString());
			cout << "\n no of players==>" << no_of_players;
		}
		void callback7()//to start the final game
		{
			fin = 1;
		}
		void start_game_renderer()
		{
			start_game = tgui::Button::create();
			GuiRendererBase::ButtonRenderer(start_game);
			start_game->setPosition(::cx(800), ::cy(480));
			start_game->setSize(::cx(190), ::cy(90));
			start_game->setText("Start Game");
			start_game->setTextSize((::cx(32) + ::cy(32)) / 2);
			start_game->getRenderer()->setBackgroundColor(sf::Color::Black);
			start_game->getRenderer()->setTextColor(sf::Color::White);
			start_game->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
			start_game->getRenderer()->setRoundedBorderRadius(10);
			start_game->getRenderer()->setOpacity(0.8);
			start_game->onPress(&graphics::loadWidgets::callback7, this);
			window4->add(start_game);
		}
		void button1_renderer()//to go from window2 to window3
		{
			button1 = tgui::Button::create();
			GuiRendererBase::ButtonRenderer(button1);
			button1->setPosition(::cx(780), ::cy(520));
			button1->setSize(::cx(150), ::cy(75));
			button1->setText("Next");
			button1->setTextSize((::cx(32) + ::cy(32)) / 2);
			button1->getRenderer()->setBackgroundColor(sf::Color::Black);
			button1->getRenderer()->setTextColor(sf::Color::White);
			button1->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
			button1->getRenderer()->setRoundedBorderRadius(10);
			button1->getRenderer()->setOpacity(0.8);
			button1->onPress(&graphics::loadWidgets::callback3, this);
			window2->add(button1);
		}
		void box1_renderer()
		{
			box1 = tgui::ComboBox::create();
			box1->setPosition(::cx(696), ::cy(315));
			box1->setSize(::cx(300), ::cy(40));
			box1->addItem("1");
			box1->setTextSize((::cx(20) + ::cy(20)) / 2);
			box1->addItem("2");
			box1->addItem("3");
			box1->addItem("4");
			box1->addItem("5");
			box1->addItem("6");
			box1->addItem("7");
			box1->setSelectedItem("1");
			box1->setChangeItemOnScroll(true);
			box1->getRenderer()->setBackgroundColor(sf::Color(51, 51, 51));
			box1->getRenderer()->setOpacity(0.8);
			box1->getRenderer()->setTextColor(sf::Color::White);
			box1->getRenderer()->setArrowColor(sf::Color::Black);
			window2->add(box1);
			
		}
		void callback1()
		{
			window1->setVisible(false);
			window2->setVisible(true);
		}
		void l1_renderer()
		{
			l1 = tgui::Label::create();
			l1->setText("How many players are playing?");
			GuiRendererBase::LabelRenderer(l1);
	
			l1->setPosition(::cx(485), ::cy(194));
			l1->setTextSize((::cx(50) + ::cy(50)) / 2);
			//l1->getRenderer()->setBackgroundColor(sf::Color::Blue);
			l1->getRenderer()->setTextColor(sf::Color::White);
			l1->getRenderer()->setBackgroundColor(sf::Color::Black);

			window2->add(l1);
		}
		void offlinePlay_renderer()
		{
			offlinePlay = tgui::Button::create();
			GuiRendererBase::ButtonRenderer(offlinePlay);
			offlinePlay->setPosition(::cx(643), ::cy(230));
			offlinePlay->setSize(::cx(500), ::cy(150));
			offlinePlay->setText("Play Offline");
			offlinePlay->getRenderer()->setTextColor(sf::Color(36, 143, 36));
			offlinePlay->getRenderer()->setBackgroundColor(sf::Color::Black);
			offlinePlay->getRenderer()->setBackgroundColorHover(sf::Color(71, 71, 107));
			offlinePlay->getRenderer()->setBackgroundColorDown(sf::Color(133, 133, 173));
			
			offlinePlay->getRenderer()->setOpacity(0.8);
			offlinePlay->getRenderer()->setRoundedBorderRadius(10);
			offlinePlay->getRenderer()->setTextSize((::cx(32) + ::cy(32)) / 2);
			offlinePlay->onPress(&graphics::loadWidgets::callback1, this);
			window1->add(offlinePlay);
			
		}
		void onlinePlay_renderer()
		{
			onlinePlay = tgui::Button::create();
			GuiRendererBase::ButtonRenderer(onlinePlay);
			onlinePlay->setPosition(::cx(643), ::cy(460));
			onlinePlay->setSize(::cx(500), ::cy(150));
			onlinePlay->setText("Play Online");
			onlinePlay->getRenderer()->setTextColor(sf::Color(36, 143, 36));
			onlinePlay->getRenderer()->setBackgroundColor(sf::Color::Black);
			onlinePlay->getRenderer()->setBackgroundColorHover(sf::Color(71, 71, 107));
			onlinePlay->getRenderer()->setBackgroundColorDown(sf::Color(133, 133, 173));

			onlinePlay->getRenderer()->setOpacity(0.8);
			onlinePlay->getRenderer()->setRoundedBorderRadius(10);
			onlinePlay->getRenderer()->setTextSize((::cx(32) + ::cy(32)) / 2);
			window1->add(onlinePlay);
		}
		
	};
	void callable()
	{
		//write event loop of sfml 
		
		window.setFramerateLimit(60);
		tgui::Gui gui(window);
			
		loadWidgets load;
		load.tex1.loadFromFile("GUI attributes/arrow.png");

		load.tex2.loadFromFile("GUI attributes/arrow1.png");

		const int no_of_frame = 39;
		vector<tgui::Picture::Ptr> back_pic(no_of_frame,NULL);
		tgui::Texture tex1;
		
		for (int i = 0; i < no_of_frame; i++)
		{
			string s1 = "Matrix_images/";
			string s2 = "frame_";
			string s4 = "_delay-0.png";
			string s3 = "";
			if (i < 10)
			{
				s3 = "0" + to_string(i);
			}
			else
			{
				s3 = to_string(i);
			}
			string fin = s1 + s2 + s3 + s4;
			tgui::Texture tex;
			
			tex.load(fin);
			back_pic[i] = tgui::Picture::create(tex);
			back_pic[i]->setVisible(false);
			back_pic[i]->setSize({ "100%","100%" });
			gui.add(back_pic[i]);
		}
		//back_pic->setSize({ "100%","100%" });
		

		
		gui.add(load.window1);
		gui.add(load.window3);
		gui.add(load.window2);
		gui.add(load.window4);
		gui.add(load.window0);
		
		load.offlinePlay_renderer();
		load.onlinePlay_renderer();
		load.l1_renderer();
		load.box1_renderer();
		load.button1_renderer();
		load.l2_renderer();
		load.editBoxRenderer();
		load.uploadFile_renderer();
		load.button2_renerer();
		load.wrongRenderer();
		load.start_game_renderer();
		load.starting_renderer();
		
		load.label0Renderer();
		load.button0Renderer();
		
		
		load.goBackWindow1Renderer();
		

		//choosing which window has to be shown first in case the path to vcvars is not uploaded
		ifstream in("path.txt", ios::in);
		char path[50];
		if (in)//file exists
		{
			string st = "";
			while (in)
			{
				in >> path;
				
				st += path;
				st += " ";
				
						
			}
			in.close();
			path_to = st;
			path_to.insert(path_to.begin(), '"');
			path_to.push_back('"');
			load.window0->setVisible(false);
			load.window1->setVisible(true);
		}
	
		else
		{
			load.window0->setVisible(true);
		}
		
		//load.window0->setVisible(true);
		
	 	//load.window1->setVisible(false);
	    //load.window2->setVisible(false);
		//load.window3->setVisible(false);
		mover_tgui<tgui::Label::Ptr> move1;
		//move1.moving(window, gui, load.starting);
		int done = 0;
		sf::Clock clock;
		sf::Time time;
		double elapsed_time=0;
		
		double animation_time = 1.5;
		int frame1 = 0;
		while (window.isOpen())
		{
			time = clock.restart();
			elapsed_time += time.asSeconds();
			nop = load.no_of_players;
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();


				if (event.type == sf::Event::Resized)
				{
					window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

				}
				gui.handleEvent(event);
			}
			
			//animating the backgrond
			back_pic[frame1]->setVisible(false);
			int frame = (int)((((double)elapsed_time / (double)animation_time)) * no_of_frame)%no_of_frame;
			frame1 = frame;
			back_pic[frame]->setVisible(true);
			

			string st = "Upload algorithms: " + to_string(load.count) + "/" + to_string(load.no_of_players);
			load.l2->setText(st);
			window.clear(sf::Color(153, 153, 102));
			gui.draw();
			window.display();
			load.starting->setVisible(false);
			if (fin == 1)
			{
				fin = 0;
				current_number = 1;
				load.starting->setVisible(true);
				window.clear(sf::Color(153, 153, 102));
				gui.draw();
				window.display();
				cout << "\n data received is==>";
				for (int i = 0; i < load.player_name.size(); i++)
				{
					cout << load.player_name[i] << " " << load.algo_path[i] << endl;
				}
				vector<string> algo_path;//original names of the fully prepared files
				for (int i = 0; i < load.algo_path.size(); i++)
				{
					cout << "\n original filename is==>" << getFilename(load.algo_path[i]);
					algo_path.push_back(prepare(load.algo_path[i]));
				}
				//tempo file
				ofstream out2("proxy_file.cpp", ios::out);
				for (int i = load.no_of_players + 1; i <= 10; i++)//to be changed because now the runner function will have the same name
				{
					string namesp = "namespace ";
					string user = "user" + to_string(i)+"\n{\n"+"void GreedMain(ship &ob)\n{\n}\n}\n";
					string comp = namesp + user;
					
					char input[100];
					strcpy(input, comp.c_str());
					out2 << input;
				}
				out2.close();
				//make the cpp file
				ofstream out("greed.cpp", ios::out);
				char main_lib[] = "#include \"lib2.hpp\"\n";
				out << main_lib;

				string st = "";
				for (int i = 0; i < algo_path.size(); i++)
				{
					string inc = "#include ";
					string inc1 = "\"";
					string inc2 = "\"\n";
					st = inc + inc1 + algo_path[i] + inc2;

					char ch[100];
					strcpy(ch, st.c_str());
					out << ch;
				}
				string inc = "#include ";
				string inc1 = "\"";
				string inc2 = "\"\n";
				string peak = inc + inc1 + "proxy_file.cpp" + inc2;
				char ch1[100];
				strcpy(ch1, peak.c_str());
				out << ch1;
				string stuff = "#include \"main.cpp\"";
				char ch2[100];
				strcpy(ch2, stuff.c_str());
				out << ch2;
				out.close();
				//compiling the greed file
				cout << "\n file compiled==>" << compile2();
				
				//preparing the system command for compiling and creating the exe file for the main game

				

				//preparing the file that the game file will read to know the number of players player name and algo path
				ofstream out1("user_info.dat", ios::out | ios::binary);
				for (int i = 0; i < load.player_name.size(); i++)
				{
					fileData ob(load.no_of_players, load.player_name[i]);
					out1.write((char*)&ob, sizeof(ob));
				}
				out1.close();
				system("greed.exe");
			}
		}
		

		
		
	}
};
int graphics::fin = 0;
int main()
{
	graphics g;
	g.callable();
	//prepare("F:\\GREED(programming game)\\GREED(programming game)\\user_algos.cpp")
	//cleaning the files from the directory
	
	remove("final_logs.txt");
	remove("greed.cpp");
	remove("greed.exe");
	remove("greed.obj");
	remove("proxy_file.cpp");
	//remove("user_info.dat");
	remove("log.txt");
	for (int i = 1; i <= nop; i++)
	{
		string name = "user" + to_string(i)+"_temp";
		remove(("user" + to_string(i) + ".obj").c_str());
		remove((name + ".cpp").c_str());
		remove((name + ".obj").c_str());
	}
	
}