#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gravity_model.cpp"
#include "GravityObjDrawer.cpp"

using Planet = GravitatingObject;
using namespace std;

int main(int argc, char const *argv[]) {
    map<string, sf::Texture> textures;

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Space Game!");

    sf::Texture background_texture;
    if (!background_texture.loadFromFile("img/background2.jpg")) {
        // error...
    }
    // background_texture.create(window.getSize().x, window.getSize().y);
    sf::Sprite background;
    background.setTexture(background_texture);
    
    background.setScale((window.getSize().x+0.0)/background_texture.getSize().x, (window.getSize().y+0.0)/background_texture.getSize().y);

    window.setFramerateLimit(120);  // call it once, after creating the window

    GravitySolver gs;  // Created the gravity modelling environment

    string texture_path = "img/rocket.png";
    sf::Texture playerTexture;
    if (!textures.count(texture_path)) {
        if (!playerTexture.loadFromFile(texture_path)) {
            // error...
        }
        textures[texture_path] = playerTexture;
    } else {
        playerTexture = textures[texture_path];
    }
    playerTexture.setSmooth(true);


    gs.player()->setTexture(playerTexture);
    gs.player()->setOrigin(gs.player()->getGlobalBounds().width / 2,
                           gs.player()->getGlobalBounds().height / 2);
    gs.player()->setScale(0.1, 0.1);

    

    GravityObjDrawer drawer(gs, window);

    sf::Sprite planetSprite;
    sf::Texture planetTexture;
    texture_path = "img/finish.png";
    if (!textures.count(texture_path)) {
        if (!planetTexture.loadFromFile(texture_path)) {
            // error...
        }
        textures[texture_path] = planetTexture;
    } else {
        planetTexture = textures[texture_path];
    }
    planetSprite.setTexture(planetTexture);
    planetSprite.setOrigin(planetSprite.getGlobalBounds().width / 2,
                           planetSprite.getGlobalBounds().height / 2);
    planetSprite.setScale(0.1, 0.1);

    

    
    gs.AddFixedObject<Planet>(true, 250, 250, 1000);  // Adding a planet
    gs.grav_objects[gs.grav_objects.size()-1]->setTexture(planetTexture);
    gs.grav_objects[gs.grav_objects.size()-1]->setOrigin(gs.grav_objects[gs.grav_objects.size()-1]->getGlobalBounds().width / 2,
                           gs.grav_objects[gs.grav_objects.size()-1]->getGlobalBounds().height / 2);
    gs.grav_objects[gs.grav_objects.size()-1]->setScale(0.1, 0.1);

    gs.AddFixedObject<Planet>(true, 1250, 550, 5000);  // And another one
    gs.grav_objects[gs.grav_objects.size()-1]->setTexture(planetTexture);
    gs.grav_objects[gs.grav_objects.size()-1]->setOrigin(gs.grav_objects[gs.grav_objects.size()-1]->getGlobalBounds().width / 2,
                           gs.grav_objects[gs.grav_objects.size()-1]->getGlobalBounds().height / 2);
    gs.grav_objects[gs.grav_objects.size()-1]->setScale(0.1, 0.1);
    
    gs.AddFixedObject<Planet>(true, 2000, 0, 3000);
    gs.grav_objects[gs.grav_objects.size()-1]->setTexture(planetTexture);
    gs.grav_objects[gs.grav_objects.size()-1]->setOrigin(gs.grav_objects[gs.grav_objects.size()-1]->getGlobalBounds().width / 2,
                           gs.grav_objects[gs.grav_objects.size()-1]->getGlobalBounds().height / 2);
    gs.grav_objects[gs.grav_objects.size()-1]->setScale(0.1, 0.1);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // get the local mouse position (relative to a window)
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);
        vec<float> mouseVector({localPosition.x - gs.player()->getPosition().x,
                                localPosition.y - gs.player()->getPosition().y});
        float angle = atan2(mouseVector.y, mouseVector.x) * 180 / 3.1415;
        gs.player()->mouse_shift = {mouseVector.x/window.getSize().x, mouseVector.y/window.getSize().x};
        drawer.setRocketRotation(angle);
        gs.step();

        window.clear();
        window.draw(background);
        drawer.draw();

        // planetSprite.setPosition(100, 100);
        // window.draw(planetSprite);

        window.display();
    }

    return 0;
}