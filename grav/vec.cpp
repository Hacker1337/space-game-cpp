// A simple 2D-vector template for concise notation
#include<cmath>
#include<iostream>

template<typename T>
class vec {
public:
	T x, y;

	vec operator+(const vec& other) {
		return {x + other.x, y + other.y};
	}
	vec& operator+=(const vec& other) {
		x += other.x; y += other.y;
		return *this;
	}
	vec operator-(const vec& other) {
		return {x - other.x, y - other.y};
	}
	vec& operator-=(const vec& other) {
		x -= other.x; y -= other.y;
		return *this;
	}
	vec operator-() {
		return {-x, -y};
	}
	vec operator*(const T& k) {
		return {x*k, y*k};
	}
	vec operator/(const T& k) {
		return {x/k, y/k};
	}
	friend vec operator*(const T& k, const vec& v) {
		return {k*v.x, k*v.y};
	}

	// vec operator*(const vec& rhs) {
	// 	// Multiplication is element-wise!!!
	// 	return {x * rhs.x, y * rhs.y};
	// }

	T modulo_squared() const {
		return x*x + y*y;
	}

	T modulo() const {
		return sqrt(modulo_squared());
	}

	friend std::ostream& operator<<(std::ostream& out, const vec& a) {
		return out<< '('<< a.x<< ", "<< a.y<< ')';
	}

	bool operator==(const vec& other) {
		return (x == other.x && y == other.y);
	}

	operator vec<int>() {
		return {static_cast<int>(x), static_cast<int>(y)};
	}
};