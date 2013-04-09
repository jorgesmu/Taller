#include "mapa.h"
#include "camara.h"
#include "../utilities/vec2.h"
#include "../utilities/coordenadas.h"

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
Tile* Mapa::getTile(int x, int y) {
	return &(tiles[x*h + y]);
}

// Chequea que el tile exista
bool Mapa::tileExists(int x, int y) const {
	return x >= 0 && y >= 0 && x < w && y < h;
}

// Getter para el vector de tiles, para recorrerlo entero
TileVec& Mapa::allTiles() {
	return tiles;
}

void Mapa::blit(SDL_Surface* dest, Camara& camara) {
	// Primero calculamos cuantos tiles nos vamos a tener que mover horizontal y verticalmente
	const int tile_count_x = (camara.getW() / Tile::TILE_ANCHO) + 2;
	const int tile_count_y = 2*((camara.getH() / Tile::TILE_ALTO) + 2);
	// Calculamos el tile de la esquina superior izquierda
	vec2<int> curr_tile = MouseCoords2Tile(vec2<int>(0, 0), camara);
	// Lo movemos uno mas para el N y al E
	curr_tile = tileWalk(curr_tile, GDIR::NE);
	//curr_tile = tileWalk(curr_tile, GDIR::E);
	// Con esta variable controlamos si cuando bajamos de fila vamos hacia el SO o hacia el el SE
	bool last_turn_SO = true;
	// Iteramos
	for(int y = 0;y < tile_count_y;y++) {
		vec2<int> fila_start = curr_tile;
		for(int x = 0;x < tile_count_x;x++) {
			// Si el tile en el que estamos parados es valido, lo blitteamos
			if(tileExists(curr_tile.x, curr_tile.y)) {
				getTile(curr_tile.x, curr_tile.y)->blit(dest, camara);
			}
			// Nos movemos para el oeste
			curr_tile = tileWalk(curr_tile, GDIR::O);
		}
		// Bajamos de fila
		if(last_turn_SO) {
			curr_tile = tileWalk(fila_start, GDIR::SE);
		}else{
			curr_tile = tileWalk(fila_start, GDIR::SO);
		}
		last_turn_SO = !last_turn_SO;
	}


	// Version que blittea recorriendo los tiles matricialmente
	//for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		//it->blit(dest, camara);
	//}
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
			getTile(x,y)->setBlitCoords((x-y)*Tile::TILE_ANCHO/2, (x+y)*Tile::TILE_ALTO/2);
		}
	}
}

// Devuelve el tile segun su posicion en el mapa dado por el pixel superior
Tile* Mapa::getTilePorPixeles(int pixelX , int pixelY) {
	Tile* retorno = NULL;
	int x;
	int y;
	int x0 = pixelX ;
	int y0 = pixelY - Tile::TILE_ALTO/2;
	x = y0 + ((x0 + Tile::TILE_ANCHO/2) / 2);
	y = y0 - ((x0 - Tile::TILE_ANCHO/2) / 2);
	x /= Tile::TILE_ALTO;
	y /= Tile::TILE_ALTO;
	retorno = this->getTile(x , y);
	return retorno;
}