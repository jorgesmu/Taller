#include "camara.h"
#include "../constants/model.h"

// Ctor
Camara::Camara() : x(0.0), y(0.0), w(0), h(0), vx(0.0), vy(0.0), margen(0) {
	// ...
}

// Dtor
Camara::~Camara() {
	// ..
}

// Settea el ancho de la camara y el ancho del margen de deslizamiento
void Camara::init(int w, int h, int marg_width) {
	this->w = w;
	this->h = h;
	this->margen = marg_width;
}

// Cambia la posicion de la camara
void Camara::mover(float x, float y) {
	this->x = x;
	this->y = y;
}

// Getters para la posicion
float Camara::getX() const {
	return x;
}

float Camara::getY() const {
	return y;
}

// Actualiza la posicion basado en la velocidad usando Euler nomas
void Camara::update() {
	x += vx * CONST_DT;
	y += vy * CONST_DT;
}

// Actualiza la velocidad basado en la posicion del mouse
void Camara::update_speed(const SDL_Rect& mouse_coords) {
	bool anymod = false;

	if(mouse_coords.x < margen) {
		vx = -DRAG_SPEED;
		anymod = true;
	}else if(mouse_coords.x > w-margen) {
		vx = DRAG_SPEED;
		anymod = true;
	}
	if(mouse_coords.y < margen) {
		vy = -DRAG_SPEED;
		anymod = true;
	}else if(mouse_coords.y > h-margen) {
		vy = DRAG_SPEED;
		anymod = true;
	}
	if(!anymod) {
		vx = vy = 0.0f;
	}
}