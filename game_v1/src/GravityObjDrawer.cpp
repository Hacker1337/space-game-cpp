// #include "gravity_model.cpp"
#include <SFML/Graphics.hpp>
#include<vector>

class GravityObjDrawer {
    const GravitySolver& gs;
    sf::RenderWindow& window;
    const float scale = 1;
    sf::Sprite fire;
    sf::Texture fire_texture;
   public:
    GravityObjDrawer() = delete;

    GravityObjDrawer(const GravitySolver& gs, sf::RenderWindow& window) : gs(gs), window(window) {

        sf::Vector2f center(window.getSize().x/2,window.getSize().y/2);
        gs.grav_objects[0]->setPosition(center);
        if (!fire_texture.loadFromFile("img/fire.png")) {
            // error...
        }
        fire.setTexture(fire_texture);
        fire.setOrigin(-10, fire_texture.getSize().y/2.0);
        fire.setPosition(center);

    }

    void draw() {
        // draw velocity vector
        vec<float> speed = gs.player()->v();
        float speed_angle = atan2(speed.y, speed.x);
        // sf::RectangleShape line(sf::Vector2f(15 + 100*atan(speed.modulo()/3), 5));
        sf::RectangleShape line(sf::Vector2f(5 + 50*sqrt(sqrt(speed.modulo())), 5));
        line.rotate(speed_angle*180/3.1416);
        line.setPosition(gs.grav_objects[0]->getPosition());
        window.draw(line);

        window.draw(fire);

        window.draw(*gs.grav_objects[0]);
        for (int i = 1; i < gs.grav_objects.size(); i++)
        {
            auto rel_coord = (gs.grav_objects[i]->r()-gs.grav_objects[0]->r())*scale;
            if 
            (2*abs(rel_coord.x) < window.getSize().x + 3*gs.grav_objects[i]->size()*scale and
             2*abs(rel_coord.y) < window.getSize().y + 3*gs.grav_objects[i]->size()*scale) 
            {
                gs.grav_objects[i]->setPosition(gs.grav_objects[0]->getPosition()+sf::Vector2f(rel_coord.x, rel_coord.y));
                window.draw(*gs.grav_objects[i]);
            }
        }

    }
    void setRocketRotation(float angle) {
        gs.grav_objects[0]->setRotation(angle);
    }
    void setFire(float angle, float module) {
        fire.setScale(-1e-3*module, 0.1);
        fire.setRotation(angle);
    }
};