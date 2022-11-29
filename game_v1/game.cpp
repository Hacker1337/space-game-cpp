#include <SFML/Graphics.hpp>
#include <iostream>
#include<fstream>
#include<vector>
#include<string>
#include"gravity_model.cpp"
#include "GravityObjDrawer.cpp"

using Planet = GravitatingObject;
using namespace std;

int main(int argc, char const *argv[])
{
	

    sf::RenderWindow window(sf::VideoMode(1000,1000), "Hello From SFML");

    window.setFramerateLimit(60); // call it once, after creating the window


    GravitySolver gs; // Created the gravity modelling environment

    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("img/rocket_white.png"))
    {
        // error...
    }
    playerTexture.setSmooth(true);
    sf::Sprite playerSprite;
    playerSprite.setTexture(playerTexture);
    playerSprite.setOrigin(playerSprite.getGlobalBounds().width/2, playerSprite.getGlobalBounds().height/2);
    playerSprite.setScale(0.1, 0.1);

    GravityObjDrawer drawer(gs, playerSprite, window);

    sf::Sprite planetSprite;
    sf::Texture planetTexture;
    if (!planetTexture.loadFromFile("img/planet.png"))
    {
        // error...
    }
    planetSprite.setTexture(planetTexture);
    planetSprite.setOrigin(planetSprite.getGlobalBounds().width/2, planetSprite.getGlobalBounds().height/2);
    planetSprite.setScale(0.1, 0.1);

	gs.player()->mouse_shift = {0, -1}; // Moved the mouse(engines started)
	for(int i = 0; i < 3; ++i) gs.step();
	gs.player()->mouse_shift = {0, 1}; // Changed direction(slowing down)
	for(int i = 0; i < 2; ++i) gs.step();
	gs.player()->mouse_shift = {-1, 0}; // Now, moving along the x axis
	for(int i = 0; i < 5; ++i) gs.step(); // Accelerating
	gs.player()->mouse_shift = {0, 0}; // Engines off
	gs.AddFixedObject<Planet>(true, 250, 250, 1000); // Adding a planet
    drawer.addObject(planetSprite);
	gs.AddFixedObject<Planet>(true, 1250, 550, 5000); // And another one
    // drawer.addObject(planetSprite);
	gs.AddFixedObject<Planet>(true, 2000, 0, 3000);
    // drawer.addObject(planetSprite);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        // get the local mouse position (relative to a window)
        sf::Vector2i localPosition = sf::Mouse::getPosition(window); 
        // std::cout << localPosition.x << " " << localPosition.y << std::endl;
        vec<float> mouseVector({localPosition.x - playerSprite.getOrigin().x, localPosition.y - playerSprite.getOrigin().y});
        float angle = atan2(mouseVector.y, mouseVector.x)*180/3.1415;
        // cout << mouseVector.x << " " << mouseVector.y << " " << angle << endl;
        drawer.setRocketRotation(angle);
        gs.step();

        window.clear();
        drawer.draw();

        // planetSprite.setPosition(100, 100);
        // window.draw(planetSprite);

        window.display();
        
    }

    return 0;
}