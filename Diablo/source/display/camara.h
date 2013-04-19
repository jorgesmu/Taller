#pragma once

#include "SDL.h"

const float DRAG_SPEED = 4.0f;

class Camara {

	private:
	float x, y; // Posicion de la camara (top left)
	int w, h; // Ancho y alto
	float vx, vy; // Velocidad de la camara
	int margen;

	public:
	// Ctor
	Camara();
	// Dtor
	~Camara();
	// Settea el ancho de la camara y el ancho del margen de deslizamiento
	void init(int w, int h, int marg_width);
	// Cambia la posicion de la camara
	void mover(float x, float y);
	// Getters para la posicion
	float getX() const;
	float getY() const;
	int getW() const;
	int getH() const;
	// Actualiza la posicion basado en la velocidad
	void update();
	// Actualiza la velocidad basado en la posicion del mouse
	void update_speed(const SDL_Rect& mouse_coords);

};