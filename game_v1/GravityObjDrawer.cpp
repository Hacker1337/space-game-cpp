// #include "gravity_model.cpp"
#include <SFML/Graphics.hpp>
#include<vector>

class GravityObjDrawer {
    const GravitySolver& gs;
    vector<sf::Sprite> object_sprites;
    sf::RenderWindow& window;
    const float scale = 1;
   public:
    GravityObjDrawer() = delete;

    GravityObjDrawer(const GravitySolver& gs, sf::Sprite playerSprite, sf::RenderWindow& window) : gs(gs), window(window) {
        object_sprites.push_back(playerSprite);

        sf::Vector2u center(window.getSize().x/2,window.getSize().y/2);
        object_sprites[0].setPosition(center.x, center.y);
        
    }

    void draw() {
        window.draw(object_sprites[0]);
        // object_sprites[0].rotate(0.5);
        for (int i = 1; i < object_sprites.size(); i++)
        {
            auto rel_coord = (gs.grav_objects[i]->r()-gs.grav_objects[0]->r())*scale;
            if (2*rel_coord.x < window.getSize().x and 2*rel_coord.y < window.getSize().y) {
                object_sprites[i].setPosition(object_sprites[0].getOrigin()+sf::Vector2f(rel_coord.x, rel_coord.y));
                window.draw(object_sprites[i]);
            }
        }
        
    }
    void setRocketRotation(float angle) {
        object_sprites[0].setRotation(angle);
    }
    void addObject(sf::Sprite spr) {
        object_sprites.push_back(spr);
    }

};