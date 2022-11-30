#include<vector>
#include<map>
#include"vec.cpp"

#include<memory>
using namespace std;

class GravitatingObject { // Stationary gravitational object abstract class
protected:
	bool erased = false;
	float mass;
	vec<float> pos;
	float radius;

	float to_pixel_coef = 1.f;
public:
	GravitatingObject() = delete;
	GravitatingObject(float x, float y, float mass, float radius):
	pos{x, y}, mass(mass), radius(radius) {}

	// May well need to allocate resources (e.g. images) far down the hierarchy
	virtual ~GravitatingObject() {}

	// Copying semantics are undesireable
	GravitatingObject(const GravitatingObject& other) = delete;
	GravitatingObject& operator=(const GravitatingObject& other) = delete;

	// Invalidating-proof access to private properties
	float m() const {
		return mass;
	}
	vec<float> r() const {
		return pos;
	}
	float x() const {
		return pos.x;
	}
	float y() const {
		return pos.y;
	}
	float size() const {
		return radius;
	}

	bool dead() const {
		return erased;
	}

	bool operator==(GravitatingObject* other) {
		// We assume the object to be uniquely identified by its position 
		return other->pos == pos; 
	}

	vec<int> convert_to_pixels() {
		return to_pixel_coef*pos;
	}

	virtual void kill() {return;}
};

enum class Bounds {TOP, RIGHT, BOTTOM, LEFT};
// Would be convenient with the "using enum " expression from c++20

class MobileGravitatingObject: public GravitatingObject {
	// Self-explanatory 
protected:
	vec<float> vel;
	//using enum Bounds;
public:
	vec<float> accel;

	MobileGravitatingObject() = delete;
	MobileGravitatingObject(float x, float y, float mass, float radius=5., float v0x = .0, float v0y = .0):
	GravitatingObject(x, y, mass, radius), vel{v0x, v0y}, accel{0, 0} {}

	vec<float> v() const {
		return vel;
	}
	float v_squared() const {
		return vel.modulo();
	}

	void set_vel(vec<float> v) {
		vel = v;
	}

	// Integrator may well need to be modified(movement animations/attenuation)
	virtual void move(const float& dt) {
		// Naive integration okay for the goals we pursue
		// VERLET
		pos += vel * dt + 0.5*accel*dt*dt;
		vel += accel * dt;
	}

	// Object-specific event-bound functions
	virtual void on_collision(shared_ptr<GravitatingObject> col_with) {
		// To process landings
		return;
	}

	virtual void on_locked(shared_ptr<GravitatingObject> locked_to) {
		// To process low-velocity states where jet acceleration is 
		// not sufficient to leave an object's gravitational field
		return;
	}

	virtual void on_out_of_bounds(Bounds b) {
		// Behaviour on crossing bound is yet to be defined
		return;
	}
};

class Projectile final: public MobileGravitatingObject {
private:
	int damage;
public:
	Projectile(float x, float y, int dmg, float mass, float radius, float vx0, float vy0): 
	MobileGravitatingObject(x, y, mass, radius, vx0, vy0), damage(dmg) {}

	void on_collision(shared_ptr<GravitatingObject> col_with) override {
		// Has to deal damage (should change the signature for that to be possible)
		// And delete itself. That is the next big problem, at the moment 
		// deleting something from grav_objects is basically unmanageable
		//col_with->take_damage(dmg) 

		//Instakill for testing
		col_with->kill();
		erased = true;
	}

	void on_out_of_bounds(Bounds b) override {
		// Should probably delete itself as well
		// Maybe not when locked (if this is even a thing)
		erased = true;
	}
};

class Spaceship: public MobileGravitatingObject {
protected:
	float jet_accel_mod;
	float proj_speed;
	int proj_damage;
	int hp;
public:
	Spaceship(float x, float y, float mass, float radius=1., float v0x = .0, float v0y = .0):
	MobileGravitatingObject(x, y, mass, radius, v0x, v0y), jet_accel_mod(2.), proj_speed(5.), proj_damage(3), hp(100) {}

	Projectile* fire_projectile() {
		auto direction = vel / vel.modulo();
		auto v0 = proj_speed * direction;
		return new Projectile(x() + radius*(1. + direction.x), y() + radius* (1. + direction.y),
					proj_damage, .01, 1., vel.x + v0.x, vel.y + v0.y);
	}

	void kill() override {
		erased = true;
	}
};

class Player final: public Spaceship {
private:
	bool locked; shared_ptr<GravitatingObject> locked_to; // To track the object it can't leave
public:
	vec<float> mouse_shift{0, 0}; // Will be captured each step at some point, for now is just set from main

	Player(float x, float y, float mass=1., float radius=1., float v0x = .0, float v0y = .0):
	// MobileGravitatingObject(x, y, mass, radius, v0x, v0y), jet_accel_mod(2.)
	Spaceship(x, y,mass, radius, v0x, v0y), mouse_shift{0., 0.},
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
private: 
	const double GAMMA = 5; 

	const double dt = .01;	//Should regulate game speed and model accuracy

	const double X_BOUND, Y_BOUND; // To check when objects tresspass
	//using enum Bounds;

	double vis_x = 500, vis_y = 500;
public:
	vector<shared_ptr<GravitatingObject>> grav_objects; // All that gravitates
	vector<shared_ptr<MobileGravitatingObject>> mobile_objects; // Separate vector of pointers to objects that move
	// The player ship is always the 0-th

	map<GravitatingObject*, unsigned> mobile_indices;

	vector<bool> incl_mask; // Shows which objects should be included in the interactions
	// Skipping some can be useful

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

	// Processing of special cases
		void resolve_collision(unsigned mob_i, unsigned all_j) {
			if (mob_index_in_grav(mob_i) == all_j) return;

			auto d = distance(mob_index_in_grav(mob_i), all_j);
			if (d < grav_objects[all_j]->size()) {
				mobile_objects[mob_i]->on_collision(grav_objects[all_j]);
			}
		}

		void resolve_out_of_bounds(unsigned mob_i) {
			float x = mobile_objects[mob_i]->x(),
				y = mobile_objects[mob_i]->y();

			if (x > X_BOUND ) {
				mobile_objects[mob_i]->on_out_of_bounds(Bounds::RIGHT);
			} else if (x < 0) {
				mobile_objects[mob_i]->on_out_of_bounds(Bounds::LEFT);
			} else if (y > Y_BOUND) {
				mobile_objects[mob_i]->on_out_of_bounds(Bounds::TOP);
			} else if (y < 0) {
				mobile_objects[mob_i]->on_out_of_bounds(Bounds::BOTTOM);
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
		for (unsigned i = 0u; i < mobile_objects.size(); ++i) {
			mobile_objects[i]->accel = {0, 0};
			for (unsigned j = 0u; j < grav_objects.size(); ++j)
			{
				unsigned Idx = mob_index_in_grav(i);
				resolve_collision(i, j);
				if (!incl_mask[j] || mob_index_in_grav(i) == j) {continue;}
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
		for (unsigned i = 0u; i < mobile_objects.size(); ++i)
		{
			mobile_objects[i]->move(dt);
			resolve_out_of_bounds(i);
			if (mobile_objects[i]->dead())
				RemoveMobileObject(i);
		}
		for (unsigned i = 0u; i < grav_objects.size(); ++i)
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

		vector<GravitatingObject*> visible() {
			vector<GravitatingObject*> res;
			for (unsigned i = 0u; i < grav_objects.size(); ++i) {
				auto dx = player()->x() - grav_objects[i]->x();
				auto dy = player()->y() - grav_objects[i]->y();
				if (dx > -vis_x/2 && dx < vis_x/2 
						&& dy > -vis_y/2 && dy < vis_y/2) {
					res.push_back(grav_objects[i].get());
				} 
			}
			return res;
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

		void AddProjectile(Projectile* p) {
			mobile_objects.push_back(shared_ptr<Projectile>(p));
			grav_objects.push_back(shared_ptr<Projectile>(nullptr));
			grav_objects.back() = mobile_objects.back();
			mobile_indices[mobile_objects.back().get()] = grav_objects.size() - 1;
			incl_mask.push_back(false);
		}
		// Admittedly, I hate that my syntax demands starting constructor params with the include flag 

		void RemoveMobileObject(unsigned mob_i) {
			unsigned gIdx = mob_index_in_grav(mob_i);
			mobile_objects.erase(mobile_objects.begin() + mob_i);
			mobile_indices.erase(mobile_objects[mob_i].get());
			for (auto &p: mobile_indices) {
				if (p.second > gIdx) p.second--;
			}
			grav_objects.erase(grav_objects.begin() + gIdx);
		}

		void RemoveFixedObject(unsigned i) {
			// This REQUIRES the object to be fixed (i.e. present in grav_objects but not in mobile_objects)
			for (auto &p: mobile_indices) {
				if (p.second > i) p.second--;
			}
			grav_objects.erase(grav_objects.begin() + i);
		}
};