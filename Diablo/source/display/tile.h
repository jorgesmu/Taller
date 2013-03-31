#pragma once

#include "entidad2.h"
#include <vector>
#include "SDL.h"

class Tile {
	private:
	std::vector<Entidad2> entidades;
	int x, y; // x,y son las coordenadas en pixels en la pantalla absoluta

	public:

	// Constantes para los tiles
	static const int TILE_ANCHO = 150;
	static const int TILE_ALTO = 75;
		
	// Ctor, no hace nada
	Tile();

	void setX(int x);

	//void setAlto(int h);

	//void setAncho(int w);

	void addEntidad(const Entidad2& ent);
	//void deleteEntidad(const Entidad& ent);

	void setBlitCoords(int x, int y);
	
	// Blittea todas las entidades en este tile (en el orden que estan en el vector)
	void blit(SDL_Surface* pantalla, const Camara& cam);

	// Libera recursos si hace falta
	void clean();
};