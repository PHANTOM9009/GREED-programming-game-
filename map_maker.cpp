/*
scene setter program1 : : : : : : - - - - - - : : : : : : : :
this program is customized to set the scene up using the mouse and keyboard, this code produces
the binary file containing the information about the tile in the form of an id and its coordinates.
the file made using this code, may not contain the tile code in order depending upon the user
this uses many shortcuts which can be customized easily just see the moving class to customize it.
next we need to get a sorted file
*/
//this program can be used to set the scene up, this program uses many shortcut keys.
#include<SFML/Graphics.hpp>
#include<iostream>
#include<stdio.h>
#include "List.h"
#include<chrono>
#include<fstream>
using namespace std;
vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
sf::RenderWindow window(sf::VideoMode(1970, 1190), "web Dictionary");

int cx(int x)
{
    // return (x*modes[0].width/1920);
    return(x);
}
int cy(int y)
{
    return(y);
    //return (y*modes[0].height/1080);
}
class vertex :public sf::Transformable
{
public:
    sf::VertexArray s;
    int n;
    void create(float w, float h, float tw, float th)//w is the width and h is the height of the rectangle
    {
        s.setPrimitiveType(sf::Quads);
        s.resize(4);

        //setting the size of the rectangle
        s[0].position = sf::Vector2f(cx(0), cy(0));
        s[1].position = sf::Vector2f(cx(w), cy(0));
        s[2].position = sf::Vector2f(cx(w), cy(h));
        s[3].position = sf::Vector2f(cx(0), cy(h));

        //setting the  coordinates for the texture
        s[0].texCoords = sf::Vector2f(0, 0);
        s[1].texCoords = sf::Vector2f(tw, 0);
        s[2].texCoords = sf::Vector2f(tw, th);
        s[3].texCoords = sf::Vector2f(0, th);
        //s[3].color = sf::Color::Red;
    }
};
class store// this class is the structure of the list  //we are not using this file now
{
public:
    int t_id;//texture id. which texture has to be added  0 for grass and 1 for ground
    vertex rect;
    sf::RenderStates s;
    void setter(int id, vertex& r, sf::RenderStates& st)
    {
        t_id = id;
        rect = r;
        s = st;
    }
};
class file
{
public:
    int id;
    float x;
    float y;
    void setter(int i, float xx, float yy)
    {
        id = i;
        x = xx;
        y = yy;
    }
};


template<typename T>
class mover_sfml
{
public:
    void moving(sf::RenderWindow& window, T& ob, sf::RenderStates& s, int pos, List<store>& p, sf::RectangleShape& rect1, sf::RectangleShape& rect2, sf::RectangleShape& rect3)//here T will be the object of the movable class of vertexArray
    {
        cout << "\n position==>" << ob.getPosition().x << " " << ob.getPosition().y;

        bool mouseClicked = false;
        bool dragging = false;
        sf::Vector2i mousepos;

        int x2, y2, x1, y1, X, Y;
        int m, n;
        int run = 0;
        int r = 0;
        int o = 0;
        bool keyboard = false;
        int flag = 0;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();

                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    mouseClicked = true;
                    //
                    //cout<<mousepos.x<<" "<<mousepos.y<<endl;
                    mousepos = sf::Mouse::getPosition(window);


                    if (mousepos.x >= ob.getPosition().x && mousepos.x <= (ob.getPosition().x + ob.s.getBounds().width) && mousepos.y >= ob.getPosition().y && mousepos.y <= (ob.getPosition().y + ob.s.getBounds().height))
                    {

                        dragging = true;

                        x2 = event.mouseButton.x - ob.getPosition().x;
                        y2 = event.mouseButton.y - ob.getPosition().y;

                    }

                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
                {
                    ob.move(0, -1);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
                {
                    ob.move(0, 1);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
                {
                    ob.move(1, 0);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
                {
                    ob.move(-1, 0);
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
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete)
                {
                    p.delAt(pos);
                    flag = 1;
                    o = 1;

                    break;
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    o = 1;
                    cout << "\n set position is==>" << ob.getPosition().x << " " << ob.getPosition().y << endl;
                    break;
                }

            }

            if (mouseClicked == true && dragging == true && flag == 0)
            {
                ob.setPosition(sf::Mouse::getPosition(window).x - x2, sf::Mouse::getPosition(window).y - y2);//problem
                run++;
                r = 1;

            }
            else if (mouseClicked == false && dragging == false && run > 0 && r == 1 && flag == 0)
            {
                cout << cx(ob.getPosition().x) << " " << cy(ob.getPosition().y) << endl;
                r = 0;
            }
            if (o == 1)
            {
                break;
            }
            s.transform = ob.getTransform();
            window.clear(sf::Color::White);
            if (flag == 0)

                for (int i = 0; i < p.howMany(); i++)
                {
                    if (i == pos)
                        continue;
                    else
                        window.draw(p[i].rect.s, p[i].s);
                }
            window.draw(ob.s, s);
            // window.draw(rect1);
            // window.draw(rect2);
            // window.draw(rect3);
            window.display();

        }


    }


};

void add(vertex& rect, sf::Texture& tex, sf::Transform& t, sf::RenderStates& s, List<store>& screen, int t_id, int x = 1910, int y = 1000)
{
    rect.create(static_cast<float>(tex.getSize().x), static_cast<float>(tex.getSize().y), static_cast<float>(tex.getSize().x), static_cast<float>(tex.getSize().y));
    rect.setPosition(cx(x), cy(y));
    t = rect.getTransform();//
 //

    s.transform = t;//
    s.texture = &tex;
    store ob;//


    ob.setter(t_id, rect, s);//
    screen.add_rear(ob);
}


int main()
{

    window.setFramerateLimit(45);
    vertex rect;

    sf::Texture tex;
    tex.loadFromFile("grass.png");//id is 0

    sf::Texture tex1;
    tex1.loadFromFile("land.png");//id is 1
    sf::Texture tex3;
    tex3.loadFromFile("ship.png");//id is 3

    sf::Texture tex2;
    tex2.loadFromFile("water5.png");// id is 2
    sf::Transform t;
    sf::RenderStates s;
    sf::View view;
    view.setSize(static_cast<sf::Vector2f>(window.getSize()));
    cout << tex2.getSize().x << " " << tex2.getSize().y;
    List<store> screen;

    /*
    rect.setPosition(800,800);
    t=rect.getTransform();
    s.transform=t;
    ob.setter(0,rect,s);
    screen.add_rear(ob);
*/
//adding stuff to the list from the file

//dont uncomment this shit no matter what happens
   /*
    int xx=16;
    int yy=120;
    for(int j=1;j<=12;j++)//row first
    {
        xx=16;
    for(int i=1;i<=24;i++)//column rendering
    {
        add(rect,tex2,t,s,screen,2,xx,yy);
        xx+=80;
    }
        yy+=80;
    }
    */
    /*
    * for adding the land at the end
    * 
    int xx = 16;
    int yy = 1080;
    for (int i = 1; i <= 24; i++)
    {
        add(rect, tex1, t, s, screen, 1, xx, yy);
        xx += 80;
    }
    */

    

    ifstream in("position1.dat", ios::binary | ios::in);
    file sp;
    while (in)
    {
        in.read((char*)&sp, sizeof(sp));
        if (in.eof())
            break;

        rect.create(static_cast<float>(tex2.getSize().x), static_cast<float>(tex2.getSize().y), static_cast<float>(tex2.getSize().x), static_cast<float>(tex2.getSize().y));
        rect.setPosition((sp.x), (sp.y));
        t = rect.getTransform();//
     //
        if (sp.id == 0)
        {
            s.texture = &tex;
        }
        else if (sp.id == 1)
        {
            s.texture = &tex1;//for the floor

        }
        else if (sp.id == 2)
        {
            s.texture = &tex2;//for the water
        }
        else if (sp.id == 3)
        {
            s.texture = &tex3;
        }
        s.transform = t;//
        //
        store ob;//


        ob.setter(sp.id, rect, s);//
        screen.add_rear(ob);



    }
    in.close();


    mover_sfml<vertex> ob1;
    std::chrono::high_resolution_clock::time_point t1;

    sf::RectangleShape rect1(sf::Vector2f(1920, 50));
    rect1.setPosition(0, 0);
    rect1.setFillColor(sf::Color::Blue);
    sf::RectangleShape rect2(sf::Vector2f(30, 1080));
    rect2.setPosition(0, 0);
    rect2.setFillColor(sf::Color::Blue);
    sf::RectangleShape rect3(sf::Vector2f(30, 1080));
    rect3.setPosition(1890, 0);
    rect3.setFillColor(sf::Color::Blue);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
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
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
            {
                add(rect, tex, t, s, screen, 0);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
            {
                add(rect, tex1, t, s, screen, 1);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
            {
                add(rect, tex2, t, s, screen, 2);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T)
            {
                add(rect, tex3, t, s, screen, 3);
            }


            //checking for double clicked;
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
            {
                //cout<<"\n entered";
                auto d = std::chrono::high_resolution_clock::now() - t1;
                t1 = std::chrono::high_resolution_clock::now();
                std::chrono::milliseconds d1(500);

                if (d <= d1)//condition for double clicked verified
                {
                    //when double clicked

                    sf::Vector2i mousepos = sf::Mouse::getPosition(window);
                    //finding out which was the image that was double clicked so tha it can be moved
                    if (screen.howMany() != 0)
                    {

                        for (int i = 0; i < screen.howMany(); i++)
                        {
                            if (mousepos.x >= screen[i].rect.getPosition().x && mousepos.x <= (screen[i].rect.getPosition().x + screen[i].rect.s.getBounds().width) && mousepos.y >= screen[i].rect.getPosition().y && mousepos.y <= (screen[i].rect.getPosition().y + screen[i].rect.s.getBounds().height))
                            {
                                ob1.moving(window, screen[i].rect, screen[i].s, i, screen, rect1, rect2, rect3);
                            }
                            else
                                continue;
                        }
                    }

                }



            }
            window.clear(sf::Color::White);

            for (int i = 0; i < screen.howMany(); i++)
                window.draw(screen[i].rect.s, screen[i].s);
            //window.draw(rect1);
            //window.draw(rect2);
            //window.draw(rect3);

            window.display();
        }


    }
    //setting the position of  the tiles
    ofstream out("position1.dat", ios::binary | ios::out);
    file sc;
    for (int i = 0; i < screen.howMany(); i++)
    {
        sc.id = screen[i].t_id;
        sc.x = screen[i].rect.getPosition().x;
        sc.y = screen[i].rect.getPosition().y;
        out.write((char*)&sc, sizeof(sc));

    }
    out.close();

   // remove("position1.dat");

}



