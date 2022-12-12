#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gravity_model.cpp"
#include "GravityObjDrawer.cpp"


using Planet = GravitatingObject;
using namespace std;

// reads positions of planets from file and loads proper texture images
void loadMap(const string& file_name, map<string, sf::Texture>& texture_map, GravitySolver& gs) {
    
    // File pointer
    fstream fin;
  
    // Open an existing file
    fin.open(file_name, ios::in);
  
    // Get the roll number
    // of which the data is required

    // Read the Data from the file
    // as String Vector
    vector<string> row;
    string line, word, temp;
    
    // skip first line with headers
    getline(fin, line);

    while (getline(fin, line)) {
        // read an entire row and
        // store it in a string variable 'line'
        
        row.clear();

        // used for breaking words
        stringstream s(line);
  

        // read every column data of a row and
        // store it in a string variable, 'word'
        while (getline(s, word, ',')) {
  
            // add all the column data
            // of a row to a vector
            row.push_back(word);
        }

        if (row.size() <= 1) {
            //end of file
            break;
        }
        if (row[0] == "Obj Type") {
            continue;
            // pass headings row
        }
        string type = row[0];
        string image_path = row[4];
        float scale_factor = stof(row[5]);
        

        // load texture, if it wasn't loaded previously
        if (!texture_map.count(image_path)) {
            sf::Texture texture;
            if (!texture.loadFromFile(image_path)) {
                // error...
            }
            texture.setSmooth(true);
            texture_map[image_path] = texture;
        }


        if (type == "Player") {
            gs.player()->setTexture(texture_map[image_path]);
            gs.player()->setOrigin(gs.player()->getGlobalBounds().width / 2,
                                gs.player()->getGlobalBounds().height / 2);
            gs.player()->setScale(scale_factor, scale_factor); 
        }
        else {
            if (type == "Planet") {
                float x = stod(row[1]);
                float y = stof(row[2]);
                float m = stof(row[3]);
                float r = stof(row[6]);

                gs.AddFixedObject<Planet>(true, x, y, m, r);  // Adding a planet
                auto planet = gs.grav_objects[gs.grav_objects.size()-1];
                // setting planet image
                planet->setTexture(texture_map[image_path]);
                planet->setOrigin(planet->getGlobalBounds().width / 2,
                                    planet->getGlobalBounds().height / 2);
                planet->setScale(scale_factor, scale_factor);
                planet->setOrigin(planet->getGlobalBounds().width / 2,
                                    planet->getGlobalBounds().height / 2);
                planet->setScale(scale_factor, scale_factor);
 
            }
            else if (type == "Shoot") {
                gs.set_projectile_texture(texture_map[image_path], scale_factor);
            }
        }

  

    }
}

int main(int argc, char const *argv[]) {
    map<string, sf::Texture> textures;

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Space Game!");
    window.setFramerateLimit(120);  // call it once, after creating the window
    GravitySolver gs;  // Created the gravity modelling environment
    GravityObjDrawer drawer(gs, window);

    loadMap("map.csv", textures, gs);

    sf::Texture background_texture;
    if (!background_texture.loadFromFile("img/background2.jpg")) {
        // error...
    }
    sf::Sprite background;
    background.setTexture(background_texture);
    background.setScale((window.getSize().x+.0)/background_texture.getSize().x, (window.getSize().y+.0)/background_texture.getSize().y);


    

    while (window.isOpen()) {
        // get the local mouse position (relative to a window)
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);
        vec<float> mouseVector({localPosition.x - window.getSize().x/2.0f,
                                localPosition.y - window.getSize().y/2.0f});
        float angle = atan2(mouseVector.y, mouseVector.x) * 180 / 3.1415;
        gs.player()->mouse_shift = {mouseVector.x/window.getSize().x, mouseVector.y/window.getSize().x};
        drawer.setRocketRotation(angle);
        drawer.setFire(angle, mouseVector.modulo());

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                sf::Vector2i localPosition = sf::Mouse::getPosition(window);
                gs.shoot();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                vec<float> coord = gs.player()->r();
                cout << coord.x << " " << coord.y << endl;
            }
        }

        gs.step();

        window.clear();
        window.draw(background);
        drawer.draw();
        window.display();
    }

    return 0;
}