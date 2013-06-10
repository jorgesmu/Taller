#pragma once

#include <string>
#include <SDL.h>
#include "../utilities/timer.h"

// Fwd Decl.
class ResMan;

// Key press interpreter
class Boton {
	private:
	// Tamaño y color del font
	int x, y, w, h;
	// Link con el resource manager
	ResMan* rm;
	std::string res_name;
	public:
    // Ctor y Dtor
    Boton();
	~Boton();
	// Init
	void init(ResMan* rm, int x, int y, const std::string& res_name);
    // Maneja el input
	// Devuelve: 
    bool handleInput(SDL_Event &event);
    // Muestra el input en la pantalla
    void show(SDL_Surface* dest);
};
