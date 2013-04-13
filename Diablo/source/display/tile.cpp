#include "tile.h"

Tile::Tile() {
}

void Tile::addEntidad(Entidad* ent){
	entidades.push_back(ent);
	ent->setTileActual(this);
}

void Tile::deleteEntidad(Entidad* ent){
	// Copiamos el vector item por item excepto el que nos interesa borrar
	std::vector<Entidad*> new_ent;
	new_ent.reserve(entidades.size());
	for(size_t i = 0;i < entidades.size();i++) {
		if(entidades[i] != ent) {
			new_ent.push_back(entidades[i]);
		}
	}
	entidades.swap(new_ent);
}

void Tile::blit(SDL_Surface* pantalla, Camara& cam){
	for(auto it = entidades.begin(); it != entidades.end(); ++it) {
		(*it)->blit(pantalla, &cam, NULL,x, y);	
	}
}

void Tile::setBlitCoords(int x, int y) {
	this -> x = x;
	this -> y = y;
}

void Tile::clean() {
	entidades.clear();
}

int Tile::getX(){
	return this -> x;
}

int Tile::getY(){
	return this -> y;
}