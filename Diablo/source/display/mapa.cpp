#include "mapa.h"
#include "camara.h"

// Ctor
Mapa::Mapa() {
	w = h = 0;
}

// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
void Mapa::resize(int w, int h) {
	this->w = w;
	this->h = h;
	tiles.resize(w*h);
	assignTileCoords();
}

// Getter para cada tile
Tile& Mapa::getTile(int x, int y) {
	return tiles[x*w + y];
}

// Getter para el vector de tiles, para recorrerlo entero
TileVec& Mapa::allTiles() {
	return tiles;
}

void Mapa::blit(SDL_Surface* dest, const Camara& camara) {
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		it->blit(dest, camara);
	}
}

void Mapa::clean() {
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		it->clean();
	}
}

void Mapa::assignTileCoords() {
	int x = 0;
	int y = 0;
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		// TODO- fix para grilla isometrica
		it->setBlitCoords(x, y);
		if(x >= w) {
			x = 0;
			y += Tile::TILE_ALTO;
		}else{
			x += Tile::TILE_ANCHO;
		}
	}
}