#include<fstream>
#include<vector>
#include<string>
#include"gravity_model.cpp"
using Planet = GravitatingObject;
using namespace std;

int main() {
	GravitySolver gs; // Created the gravity modelling environment
	gs.player()->mouse_shift = {0, -1}; // Moved the mouse(engines started)
	for(int i = 0; i < 3; ++i) gs.step();
	gs.player()->mouse_shift = {0, 1}; // Changed direction(slowing down)
	for(int i = 0; i < 2; ++i) gs.step();
	gs.player()->mouse_shift = {-1, 0}; // Now, moving along the x axis
	for(int i = 0; i < 5; ++i) gs.step(); // Accelerating
	gs.player()->mouse_shift = {0, 0}; // Engines off
	gs.AddFixedObject<Planet>(true, 250, 250, 1000); // Adding a planet
	gs.AddFixedObject<Planet>(true, 1250, 550, 5000); // And another one
	gs.AddFixedObject<Planet>(true, 2000, 0, 3000);
	

	// ofstream file("dump.traj.txt");
	// for(int i = 0; i < 1000; ++i) {
	// 	//cout<< gs.player()->accel<< '\n';
	// 	file<< gs.player()->x()<< ' '<< gs.player()->y()<< '\n';
	// 	gs.step();
	// }
	// file.close();
	return 0;
}