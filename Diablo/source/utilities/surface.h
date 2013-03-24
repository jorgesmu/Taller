#ifndef SURFACE_H
#define SURFACE_H

#include <string>

#include "SDL.h"

class Surface {

	private:
	SDL_Surface* image;
	
	public:
	// Constructor
	// No hace nada excepto inicializar surf en NULL
	Surface();
	// Destructor - no hace nada en ppio
	~Surface();
	// Carga y optimiza la surface del archivo filename
	// Tira un exception en caso de error?
	void load(const std::string& filename);
	// Destruye la superficie y libera la memoria
	void destroy();
	// Para dibujar la superficie
	// Toma como argumentos la surface de destino (aka screen) y offset coords x, y
	void blit(SDL_Surface* destination, const int x, const int y);
	// Idem anterior pero lleva un parametro extra para clippear (spritesheets)
	void blit(SDL_Surface* destination, const int x, const int y, const SDL_Rect& clip);

};

#endif