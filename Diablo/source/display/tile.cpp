#include "tile.h"

Tile::Tile() {
}

int Tile::getX(){
	return x;
}
	
int Tile::getY(){
	return y;
}

int Tile::getU(){
	return u;
}
	
int Tile::getV(){
	return v;
}
	
int Tile::getAlto(){
	return Tile::TILE_ALTO;
}

int Tile::getAncho(){
	return Tile::TILE_ANCHO;
}

void Tile::setX(int x){
	this -> x = x;
}
	
void Tile::setY(int y){
	this -> y = y;
}

void Tile::setU(int u){
	this -> u = u;
}
	
void Tile::setV(int v){
	this -> v = v;
}

//void Tile::setAlto(int h){
//	this -> h = h;
//}
//
//void Tile::setAncho(int w){
//	this -> w = w;
//}

void Tile::addEntidad(const Entidad& ent){
	(this ->entidades).push_back(ent);
}

void Tile::deleteEntidad(const Entidad& ent){
	(this ->entidades).pop_back();//saca la ultima entidad
}

void Tile::blit(SDL_Surface* pantalla, SDL_Rect destino){
	for(int i = 0; i < entidades.size(); i++){
		entidades[i].blit(pantalla, destino);	
	}
}