#pragma once

// Clase vector de 2 coordenadas, template para generalizar el type
// Ejemplo de declaracion:
// vec2<int> ivec; 
// vec2<float> fvec, fvec2(5.0, 6.0);
// Tiene las operaciones basicas
template <typename T>
struct vec2 {
	// Miembros
	T x, y;
	// Metodos
	// Def ctor
	vec2() : x(0), y(0) { }
	// Ctor
	vec2(T x, T y) : x(x), y(y) { }
	vec2(const vec2& o) : x(o.x), y(o.y) { }
	// Dtor no hace falta
	// Los getters no hacen falta, es publico
};

template <typename T>
vec2<T> operator+(const vec2<T>& a, const vec2<T>& b) {
	return vec2<T>(a.x+b.x, a.y+b.y);
}

template <typename T>
vec2<T> operator-(const vec2<T>& a, const vec2<T>& b) {
	return vec2<T>(a.x-b.x, a.y-b.y);
}

template <typename T, typename Y>
vec2<T> operator*(Y a, const vec2<T>& b) {
	return vec2<T>(a*b.x, a*b.y);
}