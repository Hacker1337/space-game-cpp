// A simple 2D-vector template for concise notation
#include<iostream>
#include<cmath>
//#include"vec.h"

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

	T modulo_squared() {
		return x*x + y*y;
	}

	T modulo() {
		return sqrt(modulo_squared());
	}

	friend std::ostream& operator<<(std::ostream& out, const vec& a) {
		return out<< '('<< a.x<< ", "<< a.y<< ')';
	}

	bool operator==(const vec& other) {
		return (x == other.x && y == other.y);
	}
};