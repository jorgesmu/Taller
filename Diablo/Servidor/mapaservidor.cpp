#include "mapaservidor.h"


// Ctor
MapaServidor::MapaServidor() {
	w = h = 0;
}

// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
void MapaServidor::resize(int w, int h) {
	this->w = w;
	this->h = h;
	tiles.resize(w*h);
}

// Getter para cada tile
TileServidor* MapaServidor::getTile(int x, int y) {
	return &(tiles[x*h + y]);
}

// Chequea que el tile exista
bool MapaServidor::tileExists(int x, int y) const {
	return x >= 0 && y >= 0 && x < w && y < h;
}

// Getter para el vector de tiles, para recorrerlo entero
TileVecServidor& MapaServidor::allTiles() {
	return tiles;
}

void MapaServidor::getSize(int* w, int* h) const {
	*w = this->w;
	*h = this->h;
}