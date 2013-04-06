#pragma once

#include "tile.h"
#include <vector>
#include "SDL.h"

typedef std::vector<Tile> TileVec;

class Camara;

class Mapa {
	private:
	TileVec tiles; // Vector de tiles para el mapa
	int w, h;
	// Assigna a los tiles las coordenadas
	// Esta funcion es privada y se llama automaticamente cuando se hace un resize del mapa
	// (en teoria deberia ser una sola vez, cuando ser carga el mapa)
	void assignTileCoords();

	public:
	// Ctor
	Mapa();
	// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
	void resize(int w, int h);
	// Getter para cada tile
	Tile& getTile(int x, int y);
	// Verifica si el tile existe
	bool tileExists(int x, int y) const;
	// Getter para el vector de tiles, para recorrerlo entero
	TileVec& allTiles();
	// Blittea todo el mapa
	void blit(SDL_Surface* dest, const Camara& camara);
	// Libera los recursos
	void clean();
};