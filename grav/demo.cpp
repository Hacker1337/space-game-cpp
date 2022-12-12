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
	gs.AddFixedObject<Planet>(true, 0, r, m, 5.); // Adding a planet
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

void single_shot(int n_steps, string tag="projectile_demo") {
	GravitySolver gs;
	gs.player()->set_pos({500, 500});
	gs.AddFixedObject<Planet>(true, 600, 600, 1000, 5.);
	gs.player()->set_vel({.25, 0});
	//for (int i = 0; i < 1000; ++i) gs.step();
	// Same sequence as going to be called on mouse button down
	Projectile* proj = gs.player()->fire_projectile();
	gs.AddProjectile(proj);
	gs.player()->set_vel({0, 0});
	gs.player()->set_pos({0, 0});

	ofstream file("demos/" + tag + ".txt");
	file<< tag<< '\n';
	file << "1 planets at:\n"<< 600<< ' '<< 600<< "\nTrajectory:\n";
	for(int i = 0; i < n_steps; ++i) {
		file<< proj->x()<< ' '<< proj->y()<< '\n';
		gs.player()->set_vel({0, 0});
		gs.step();
	}
	file.close();
	man.add(tag);
}

void shootoff() {
	GravitySolver gs;
	gs.AddMobileObject<Spaceship>(false, 100, 100, 1., 1.);
	gs.player()->set_vel({1, 1});
	Projectile* proj = gs.player()->fire_projectile();
	gs.player()->set_vel({-100, -100});
	gs.AddProjectile(proj);
	gs.AddFixedObject<Planet>(true, 500, 500, 100., 10.);
	cout<< "# of GravObjs: "<< gs.grav_objects.size()<< '\n';
	for (auto go: gs.grav_objects) {
		cout<< go->r()<< '\n';
	}

	ofstream file("demos/Shootoff.txt");
	file<< "Shootoff\n";
	file << "2 planets at:\n100 100\n500 500\nTrajectory:\n";
	for (int i = 0; i < 50000; ++i)
	{
		file << proj->x()<< ' '<< proj->y()<< '\n';
		gs.step();
	}
	cout<< "# of GravObjs: "<< gs.grav_objects.size();
	for (auto go: gs.grav_objects) {
		cout<< go->r()<< '\n';
	}
	file.close();
	man.add("Shootoff");
}

int main(int argc, char **argv) {
	int n_steps;
	if (argc >= 1) 
		n_steps = atoi(argv[1]);
	if (n_steps == 0) n_steps = 10000;
	cout<< "Making "<< n_steps<< " steps\n";

	single_planet(n_steps, "Falling", 0);
	single_planet(n_steps, "Circle trajectory", 1);
	single_planet(n_steps, "Low velocity", .5);
	single_planet(n_steps, "High velocity", 1.5);
	single_shot(n_steps, "Projectile");
	shootoff();
	man.write();
	return 0;
}