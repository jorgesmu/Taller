#include "tile.h"

Tile::Tile() {
}

void Tile::addEntidad(const Entidad2& ent){
	(this ->entidades).push_back(ent);
}

void Tile::blit(SDL_Surface* pantalla, const Camara& cam){
	for(auto it = entidades.begin(); it != entidades.end(); ++it) {
		it->blit(pantalla, cam, x, y);	
	}
}

void Tile::setBlitCoords(int x, int y) {
	this->x = x;
	this->y = y;
}

void Tile::clean() {
	//
}