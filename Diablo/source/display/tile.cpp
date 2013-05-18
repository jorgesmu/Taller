#include "tile.h"
#include "mapa.h"

Tile::Tile() {
}

//deprecated
void Tile::addEntidad(Entidad* ent){
	entidades.push_back(ent);
	//ent->setTileActual(this);
}

void Tile::addEntidad(Entidad* ent , Mapa* mapa){
	entidades.push_back(ent);
	ent->setTileActual(this , mapa);
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

std::vector<Entidad*> Tile::getEntidades(){
	return this->entidades;
}

void Tile::blit(SDL_Surface* pantalla, Camara& cam, Personaje* personaje, Mapa* mapa){

	Tile* tilePersonaje = mapa->getTilePorPixeles(personaje->getX(), personaje->getY());
	int deltaX = personaje -> getXAnclajeNiebla() - x;
	int deltaY = personaje -> getYAnclajeNiebla() - y;
	if((abs(deltaX) <= Personaje::RADIO_VISION_X) && (abs(deltaY) <= Personaje::RADIO_VISION_Y)){
		//dibujo en colores normales
		for(auto it = entidades.begin(); it != entidades.end(); ++it) {
			(*it)->setColor(true,u,v);
			(*it)->blit(pantalla, &cam, NULL,x, y);
			(*it)->setDibujada(true);
		}
		std::vector<Tile*> tilesExplorados = personaje->getTilesExplorados();
		//si no estaba en los explorados lo agrego
		if (std::find(tilesExplorados.begin(), tilesExplorados.end(), this) == tilesExplorados.end()){
			personaje->agregarTilesExplorados(this);
		}
	}else{
		std::vector<Tile*> tilesExplorados = personaje->getTilesExplorados();
		
		if(std::find(tilesExplorados.begin(), tilesExplorados.end(), this) != tilesExplorados.end()){
			//lo bliteo en gris
			for(auto it = entidades.begin(); it != entidades.end(); ++it) {
				if((!((*it)->getDibujada())) || ((*it)->get_nombre() == "tierraDefault")){
					(*it)->setColor(false,u,v);
					(*it)->blit(pantalla, &cam, NULL,x, y);
					(*it)->setDibujada(true);
				}else{
					(*it)->blit(pantalla, &cam, NULL,x, y);
				}
			}
		}
	}

	//bliteo el personaje por si el ancla no estaba en el radio (PASA SI RADIO ES 1)pero el personaje se blitea arriba de las cosas
	//Tile* tileAnclaPersonaje = personaje->getTileAncla();
	//for(auto it = tileAnclaPersonaje->entidades.begin(); it != tileAnclaPersonaje->entidades.end(); ++it){
	//	if((*it) == personaje){
	//		(*it)->blit(pantalla, &cam, NULL, x, y);
	//	}
	//}

	//asi estaba antes
	//for(auto it = entidades.begin(); it != entidades.end(); ++it) {
	//	(*it)->blit(pantalla, &cam, NULL,x, y);	
	//}
}

void Tile::setBlitCoords(int x, int y) {
	this -> x = x;
	this -> y = y;
}

void Tile::setLogicCoords(int u, int v) {
	this -> u = u;
	this -> v = v;
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

bool Tile::sinEntidades() const {
	return entidades.size() == 0;
}

int Tile::getU() {
	return this -> u;
}

int Tile::getV(){
	return this -> v;
}

bool Tile::isCaminable(){
	for (auto it = entidades.begin(); it != entidades.end(); ++it){
		if((*it)->isCaminable() == false){
			return false;
		}
	}
	return true;
}