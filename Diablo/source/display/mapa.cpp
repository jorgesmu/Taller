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
	int x, y;
	for(x = 0;x < w;x++) {
		for(y = 0;y < h;y++) {
			getTile(x,y).setBlitCoords((x-y)*Tile::TILE_ANCHO/2, (x+y)*Tile::TILE_ALTO/2);
		}
	}
}