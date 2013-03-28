#pragma once

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
	// Devuelve true o false para success of fail
	bool load(const std::string& filename);
	// Destruye la superficie y libera la memoria
	void destroy();
	// Getters para las propiedades de la imagen
	int height() const;
	int width() const;
	// Para dibujar la superficie
	// Toma como argumentos la surface de destino (aka screen) y offset coords x, y
	void blit(SDL_Surface* destination, int x, int y);
	// Idem anterior pero lleva un parametro extra para clippear (spritesheets)
	void blit(SDL_Surface* destination, int x, int y, SDL_Rect& clip);
	// Retorna el Surface contenido.
	SDL_Surface* getSDL_Surface();
};