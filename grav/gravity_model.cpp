#include<vector>
#include<map>
#include"vec.cpp"

#include<memory>
using namespace std;

class GravitatingObject { // Stationary gravitational object abstract class
protected:
	float mass;
	vec<float> pos;
	float radius;
public:
	GravitatingObject() = delete;
	GravitatingObject(float x, float y, float mass=1):
	mass(mass), pos{x, y} {}

	// May well need to allocate resources (e.g. images) far down the hierarchy
	virtual ~GravitatingObject() {}

	// Copying semantics are undesireable
	GravitatingObject(const GravitatingObject& other) = delete;
	GravitatingObject& operator=(const GravitatingObject& other) = delete;

	// Invalidating-proof access to private properties
	float m() {
		return mass;
	}
	vec<float> r() {
		return pos;
	}
	float x() {
		return pos.x;
	}
	float y() {
		return pos.y;
	}
	float size() {
		return radius;
	}

	bool operator==(GravitatingObject* other) {
		// We assume the object to be uniquely identified by its position 
		return other->pos == pos; 
	}

	// Maybe add a virtual function to update state parameters?
	// E.g. animations or modifiable mass
};

class MobileGravitatingObject: public GravitatingObject {
	// Self-explanatory 
private:
	vec<float> vel;
public:
	vec<float> accel;

	MobileGravitatingObject() = delete;
	MobileGravitatingObject(float x, float y, float v0x = .0, float v0y = .0, float mass=1.):
	GravitatingObject(x, y, mass), vel{v0x, v0y}, accel{0, 0} {}

	vec<float> v() {
		return vel;
	}
	float v_squared() {
		return vel.modulo();
	}

	// Integrator may well need to be modified(movement animations/attenuation)
	virtual void move(const float& dt) {
		// Naive integration okay for the goals we pursue
		pos += vel * dt + 0.5*accel*dt*dt;
		vel += accel * dt;
	}

	// Object-specific event-bound functions
	virtual void on_collision() {
		// To process landings
		return;
	}

	virtual void on_locked(shared_ptr<GravitatingObject> locked_to) {
		// To process low-velocity states where jet acceleration is 
		// not sufficient to leave an object's gravitational field
		return;
	}
};

// If we end up adding enemies, probably most of the player class 
// should be shifted to an intermediary "spaceship" class

class Player final: public MobileGravitatingObject {
private:
	float jet_accel_mod; // Likely needs further balancing
	bool locked; shared_ptr<GravitatingObject> locked_to; // To track the object it can't leave
public:
	vec<float> mouse_shift{0, 0}; // Will be captured each step at some point, for now is just set from main

	Player(float x, float y, float v0x = .0, float v0y = .0, float mass=1.):
	MobileGravitatingObject(x, y, v0x, v0y, mass), jet_accel_mod(2.), mouse_shift{0., 0.},
	locked(false), locked_to(shared_ptr<GravitatingObject>(nullptr)) {}

	vec<float> add_jet_accel() {
		upd_mouse_shift(); // A suitable place to update the mouse_shift field
		// For now, applying a fixed force in the direction specified by the mouse 
		if (mouse_shift == vec<float>{0, 0}) return {0, 0};

		vec<float> jet_accel = jet_accel_mod* (-mouse_shift)/mouse_shift.modulo(); 
		accel += jet_accel;
		return jet_accel;
	}

	void upd_mouse_shift() { 
		// When locked onto object, orbit it as high as possible until player resolves the lock-on
		if (locked) mouse_shift = locked_to->r() - pos; 
	}

	void on_locked(shared_ptr<GravitatingObject> new_locked_to) override {
		if (locked && new_locked_to == locked_to) return;
		locked = true;
		this->locked_to = new_locked_to;
	}
	void unlock() { 
		locked = false;
		locked_to = nullptr;
	}
};

class GravitySolver final {
	/*
	/	Singleton, manages the whole environment of gravitating objects
	*/
private: 
	const double GAMMA = 5; // Strictly speaking, this just changes 
							// the characteristic masses of planets
							// we will need to set for same results

	const double dt = .1;	//Should regulate game speed, I haven't really been able to test this

	const double X_BOUND, Y_BOUND; // To check when objects tresspass
public:
	vector<shared_ptr<GravitatingObject>> grav_objects; // All that gravitates
	vector<shared_ptr<MobileGravitatingObject>> mobile_objects; // Separate vector of pointers to objects that move
	// The player ship is always the 0-th

	map<GravitatingObject*, unsigned> mobile_indices;

	vector<bool> incl_mask; // Shows which mobile objects should interact with each other
	// As in, all will interact with the fixed objects, but skipping 
	// interactions between light ships should somewhat boost performance

	unsigned long long epoch = 0; // To keep track of time if we don't elsewhere

	GravitySolver(): X_BOUND(1000), Y_BOUND(1000) {
		AddMobileObject<Player>(false, 0, 0); // There is always the player
	}
	GravitySolver(unsigned w, unsigned h): X_BOUND(w), Y_BOUND(h) {
		AddMobileObject<Player>(false, 0, 0); // There is always the player
	}
public:
	// Once again, no copying semantics
	GravitySolver(const GravitySolver&) = delete;
	GravitySolver& operator=(const GravitySolver&) = delete;

	~GravitySolver() = default;

	// Direct measurements 
		float distance(unsigned i, unsigned j) {
			return (grav_objects[i]->r() - grav_objects[j]->r()).modulo();
		}

		float distance_squared(unsigned i, unsigned j) {
			return (grav_objects[i]->r() - grav_objects[j]->r()).modulo_squared();
		}

		vec<float> force(unsigned i, unsigned j) { 
		// Not really used anywhere at the moment
			float sc = GAMMA * mobile_objects[i]->m() *
			 				grav_objects[j]->m() / distance_squared(i, j); 
			return sc/distance(i, j) * (mobile_objects[i]->r() - grav_objects[j]->r());
		}

	// Processing of special cases
		void resolve_collision(unsigned i, unsigned j) {
			auto d = distance_squared(i, j);
			if (d < grav_objects[i]->size() ||
				d < grav_objects[j]->size()) {
					cout<< "Collision between objects at idx "<< i<< " "<< j<< '\n';
			}
		}

		void resolve_out_of_bounds(unsigned i) {
		// Currently just floods the terminal
			if (mobile_objects[i]->x() > X_BOUND ) {
				cout<< "Object at id "<< i<< "violated the right bound";
			} else if (mobile_objects[i]->x() < 0) {
				cout<< "Object at id "<< i<< "violated the left bound";
			} else if (mobile_objects[i]->y() > Y_BOUND) {
				cout<< "Object at id "<< i<< "violated the top bound";
			} else if (mobile_objects[i]->y() < 0) {
				cout<< "Object at id "<< i<< "violated the bottom bound";
			}
		}

		// Only for the player ship for now
		void resolve_locked(unsigned i) {
			// auto vsq = player()->v_squared();
			// float d;
			// if (vsq == 0.) d = distance(0, i);
			// else d = 0.25 /GAMMA *player()->m()/grav_objects[i]->m()* vsq* vsq;
			// if (d < GAMMA* grav_objects[i]->m() ) {
			// 	player()->on_locked(grav_objects[i]);
			// }
		}

	// System-of-bodies-level calculations
	void calculate_accels() {
		for (int i = 0; i < mobile_objects.size(); ++i) {
			mobile_objects[i]->accel = {0, 0};
			for (int j = 0; j < grav_objects.size(); ++j)
			{
				unsigned Idx = mob_index_in_grav(i);
				if (incl_mask[i] || mob_index_in_grav(i) == j) {continue;}
				resolve_collision(i, j);
				float sc = GAMMA * grav_objects[j]->m() / distance_squared(Idx, j) /distance(Idx, j);
				auto dr = grav_objects[j]->r() - grav_objects[Idx]->r();
				mobile_objects[i]->accel += sc * dr;
			}
		}
		player()->add_jet_accel();
	}

	void step() {
		epoch++;
		calculate_accels();
		for (int i = 0; i < mobile_objects.size(); ++i)
		{
			mobile_objects[i]->move(dt);
			resolve_out_of_bounds(i);
		}
		for (int i = 0; i < grav_objects.size(); ++i)
		{
			resolve_locked(i);
		}
	}

	 // Access to GravitatingObjects (by regular pointers)
		vector<MobileGravitatingObject*> shifted_objects() {
			vector<MobileGravitatingObject*> res;
			for (unsigned i = 0u; i < mobile_objects.size(); ++i) {
				res.push_back(mobile_objects[i].get());
			}
			return res;
		}

		Player* player() {
			return dynamic_cast<Player*>(mobile_objects.front().get());
		} 

		unsigned mob_index_in_grav(unsigned mob_i) {
			// Converts the index of an obj in mobile_objects to 
			// the corresponding index in grav_objects
			return mobile_indices[mobile_objects[mob_i].get()];
		}
	
	// Changing the composition of the object lists

		template<typename T, class... Params>
		void AddMobileObject(bool incl, Params... args) {
			mobile_objects.push_back(make_shared<T>(args...));
			grav_objects.push_back(shared_ptr<T>(nullptr));
			grav_objects.back() = mobile_objects.back();
			mobile_indices[mobile_objects.back().get()] = grav_objects.size() - 1;
			incl_mask.push_back(incl);
		}

		template<typename T, class... Params>
		void AddFixedObject(bool incl, Params... args) {
			grav_objects.push_back(make_shared<T>(args...));
			incl_mask.push_back(incl);
		}

		// Admittedly, I hate that my syntax demands starting constructor params with the include flag 
};