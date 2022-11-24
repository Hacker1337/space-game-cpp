#include <SFML/Graphics.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Hello From SFML");
    // sf::CircleShape shape(100.f);
    // shape.setFillColor(sf::Color::Magenta);

    window.setFramerateLimit(60); // call it once, after creating the window

    // creating first sprite
    sf::Texture texture;
    if (!texture.loadFromFile("rocket_img.png"))
    {
        // error...
    }
    texture.setSmooth(true);
    sf::Sprite sprite;
    sprite.setTexture(texture);

    // position
    sprite.setPosition(sf::Vector2f(500.f, 500.f)); // absolute position
    sprite.move(sf::Vector2f(5.f, 10.f)); // offset relative to the current position


    // scale
    sprite.setScale(sf::Vector2f(0.5f, 0.5f)); // absolute scale factor
    // sprite.scale(sf::Vector2f(1.5f, 3.f)); // factor relative to the current scale

    // set origin to center
    sprite.setOrigin(150, 150);
    std::cout << sprite.getTextureRect().height;
    sprite.setOrigin(sprite.getTextureRect().height/2, sprite.getTextureRect().width/2);


    // rotation
    sprite.setRotation(20.f); // absolute angle
    // sprite.rotate(15.f); // offset relative to the current angle


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
        sf::Vector2i localPosition = sf::Mouse::getPosition(window); // window is a sf::Window
        std::cout << localPosition.x << " " << localPosition.y << std::endl; 

        sprite.rotate(1);
        window.clear();
        // window.draw(shape);

        window.draw(sprite);


        window.display();
        
    }

    return 0;
}