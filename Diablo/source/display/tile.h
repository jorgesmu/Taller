#pragma once

#include "entidad.h"
#include <vector>
#include "SDL.h"

class Tile {
	private:
	std::vector<Entidad*> entidades;
	int x, y; // x,y son las coordenadas en pixels en la pantalla absoluta

	public:

	// Constantes para los tiles
	static const int TILE_ANCHO = 100;
	static const int TILE_ALTO = 50;
		
	// Ctor, no hace nada
	Tile();
	void addEntidad(Entidad* ent);
	void deleteEntidad(Entidad* ent);
	void setBlitCoords(int x, int y);
	// Blittea todas las entidades en este tile (en el orden que estan en el vector)
	void blit(SDL_Surface* pantalla, Camara& cam);
	// Libera recursos si hace falta
	void clean();
	
	unsigned int getX();

	unsigned int getY();
};