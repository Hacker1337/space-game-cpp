#include<fstream>
#include<vector>
#include<string>
#include"gravity_model.cpp"
using Planet = GravitatingObject;
using namespace std;

struct Manifest {
	vector<string> list;

	void add(string tag) {
		list.push_back(tag);
	}

	void write() {
		ofstream file("demos/demo_man.txt");
		for (string tag: list) 
			file<< tag<<'\n';
		file.close();
	}
};
Manifest man {};

void single_planet(int n_steps, string tag="gravity_demo", float vel_coef = 1) {
	float r = 500, m = 1000;
	GravitySolver gs; // Created the gravity modelling environment
	gs.AddFixedObject<Planet>(true, 0, r, m); // Adding a planet
	float vst = sqrt(5*m/r);
	if (vel_coef != 1.f) vst *= vel_coef;
	gs.player()->set_vel({vst, 0}); // Corresponding to current orbit

	ofstream file("demos/" + tag + ".txt");
	file<< tag<< '\n';
	file << "1 planets at:\n0 "<< r<< "\nTrajectory:\n";
	for(int i = 0; i < n_steps; ++i) {
		file<< gs.player()->x()<< ' '<< gs.player()->y()<< '\n';
		gs.step();
	}
	file.close();
	man.add(tag);
}

int main(int argc, char **argv) {
	int n_steps;
	if (argc >= 1) 
		n_steps = atoi(argv[1]);
	if (n_steps == 0) n_steps = 10000;

	single_planet(n_steps, "Circle trajectory", 1);
	single_planet(n_steps, "Low velocity", .5);
	single_planet(n_steps, "High velocity", 1.5);
	man.write();
	return 0;
}