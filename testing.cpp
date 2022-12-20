#include<TGUI/Backend/SFML-Graphics.hpp>
#include<TGUI/TGUI.hpp>
#include<SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include<unordered_map>
#include <TGUI/Cursor.hpp>
using namespace std;
//gui for the software
enum class Policy { Automatic, Always, Never };
class gui_renderer
{
public:
    void callable(sf::RenderWindow& window, tgui::Gui& gui)
    {


    }
};

void login(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password)
{
    std::cout << "Username: " << username->getText() << std::endl;
    std::cout << "Password: " << password->getText() << std::endl;

}

void updateTextSize(tgui::BackendGui& gui) {
    const float windowsHeight = gui.getView().getRect().height;
    gui.setTextSize(static_cast<unsigned int>(0.08f * windowsHeight));

}
void RunGUI(tgui::Gui& gui)
{
    tgui::Theme theme{ "C:/TGUI-1.0-nightly-VisualStudio-64bit-for-SFML-2.5.1/TGUI-1.0-nightly/themes/BabyBlue.txt" };
    gui.add(tgui::Picture::create("C:/Users/asus/Downloads/background.jpg"));
    auto menu = tgui::MenuBar::create();
    menu->setRenderer(theme.getRenderer("Menubar"));
    menu->setHeight(22.f);
    menu->addMenu("File");

    menu->addMenuItem("load");
    menu->addMenuItem("Save");
    menu->addMenuItem("Exit");
    menu->addMenu("Edit");
    menu->addMenuItem("Copy");
    menu->addMenuItem("Paste");
    menu->addMenu("Help");
    menu->addMenuItem("About");
    gui.add(menu);

}


int main()
{
    

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Brain Maps");
    window.setFramerateLimit(40);
    tgui::Gui gui(window);
    RunGUI(gui);
    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);
            sf::CircleShape shape(50.f);
            shape.setFillColor(sf::Color(100, 250, 50));
            if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

            }
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        gui.draw();

        window.display();
    }



}