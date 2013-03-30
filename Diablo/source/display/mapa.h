#pragma once

#include "tile.h"
#include <vector>
#include "SDL.h"

typedef std::vector<Tile> TileVec;

class Mapa {
	private:
	TileVec tiles; // Vector de tiles para el mapa
	int w, h;
	public:
	// Ctor
	Mapa();
	// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
	void resize(int w, int h);
	// Getter para cada tile
	Tile& getTile(int x, int y);
	// Getter para el vector de tiles, para recorrerlo entero
	TileVec& allTiles();
};