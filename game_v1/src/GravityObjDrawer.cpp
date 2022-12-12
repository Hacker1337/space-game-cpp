// #include "gravity_model.cpp"
#include <SFML/Graphics.hpp>
#include<vector>

class GravityObjDrawer {
    const GravitySolver& gs;
    sf::RenderWindow& window;
    const float scale = 1;
   public:
    GravityObjDrawer() = delete;

    GravityObjDrawer(const GravitySolver& gs, sf::RenderWindow& window) : gs(gs), window(window) {

        sf::Vector2u center(window.getSize().x/2,window.getSize().y/2);
        gs.grav_objects[0]->setPosition(center.x, center.y);
        
    }

    void draw() {
        window.draw(*gs.grav_objects[0]);
        // object_sprites[0].rotate(0.5);
        for (int i = 1; i < gs.grav_objects.size(); i++)
        {
            auto rel_coord = (gs.grav_objects[i]->r()-gs.grav_objects[0]->r())*scale;
            if (2*rel_coord.x < window.getSize().x and 2*rel_coord.y < window.getSize().y) {
                gs.grav_objects[i]->setPosition(gs.grav_objects[0]->getOrigin()+sf::Vector2f(rel_coord.x, rel_coord.y));
                window.draw(*gs.grav_objects[i]);
            }
        }
        
    }
    void setRocketRotation(float angle) {
        gs.grav_objects[0]->setRotation(angle);
    }
};