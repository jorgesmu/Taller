#include "tile.h"
#include "mapa.h"
#include "../net/bitstream.h"
#include "../net/defines.h"
#include "../../Cliente/clientsocket.h"
extern PjeManager pjm;
extern ClientSocket sock;

Tile::Tile() {
	force_no_caminable = false;
}

void Tile::insertarEntidadOrdenada(Entidad* ent){
	std::vector<Entidad*> new_ent;
	new_ent.reserve(entidades.size());
	bool insertado = false;
	for(size_t i = 0; i < entidades.size();i++) {
		// trato de insertar la entidad parametro
		if(!insertado){
			if((entidades[i] != ent) && 
				(entidades[i] -> getOrdenBliteo() > ent -> getOrdenBliteo())) {
				new_ent.push_back(ent);
				insertado = true;
			} else {
				if((entidades[i] != ent) && 
				   (entidades[i] -> getOrdenBliteo() == ent -> getOrdenBliteo()) && 
				   (entidades[i] -> getX() >= ent -> getX()) && 
				   (entidades[i] -> getY() >= ent -> getY())) {
					new_ent.push_back(ent);
					insertado = true;
				}
			}
		}
		// inserto la anterior
		new_ent.push_back(entidades[i]);
	}
	// si no se pudo insertar se inserta al final
	if (!insertado) {
		new_ent.push_back(ent);
	}
	// intercambio la lista
	entidades.swap(new_ent);
}

//deprecated
void Tile::addEntidad(Entidad* ent){
	insertarEntidadOrdenada(ent);
	//entidades.push_back(ent);
	
	//ent->setTileActual(this);
}

void Tile::addEntidad(Entidad* ent , Mapa* mapa){
	insertarEntidadOrdenada(ent);
	//entidades.push_back(ent);
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

void Tile::setearExplorados(int tileX, int tileY, Personaje* personaje, Mapa* mapa){
	personaje->agregarTilesExplorados(mapa->getTile(tileX, tileY));
}

void Tile::blit(SDL_Surface* pantalla, Camara& cam, Personaje* personaje, Mapa* mapa){

	Tile* tilePersonaje = mapa->getTilePorPixeles(personaje->getX(), personaje->getY());
	int deltaX = personaje -> getXAnclajeNiebla() - x;
	int deltaY = personaje -> getYAnclajeNiebla() - y;
	if((abs(deltaX) <= personaje->getRadioX()) && (abs(deltaY) <= personaje->getRadioY())){
		//dibujo en colores normales
		for(auto it = entidades.begin(); it != entidades.end(); ++it) {
			if ( (!(*it)-> isInTile(this->x , this -> y)) && (!(*it)->isCompartido())){
				// es un ancla
				deltaX = personaje -> getXAnclajeNiebla() - (*it)->getX();
				deltaY = personaje -> getYAnclajeNiebla() - (*it)->getY();
				if((abs(deltaX) <= personaje->getRadioX()) && (abs(deltaY) <= personaje->getRadioY())){
					(*it)->setColor(true,x,y, mapa, (personaje -> getXAnclajeNiebla()), (personaje -> getYAnclajeNiebla()), personaje);
					(*it)->blit(pantalla, &cam, NULL,x, y,true);
					(*it)->setDibujada(true, mapa,personaje);				
				}
			}else{
				(*it)->setColor(true,x,y, mapa, (personaje -> getXAnclajeNiebla()), (personaje -> getYAnclajeNiebla()), personaje);
				(*it)->blit(pantalla, &cam, NULL,x, y,true);
				(*it)->setDibujada(true, mapa,personaje);			
			}
		}

		BitStream bs;
		bs << PROTO::NIEBLA_SYNC << this->u << this->v;
		sock.send(bs.str());

		personaje->agregarTilesExplorados(this);
	}else{
		std::vector<Tile*> tilesExplorados = personaje->getTilesExplorados();

		if(std::find(tilesExplorados.begin(), tilesExplorados.end(), this) != tilesExplorados.end()){
			//lo bliteo en gris
			for(auto it = entidades.begin(); it != entidades.end(); ++it) {
				if((*it)->noDibujaFueraDelRadio()) continue;

				if((!((*it)->getDibujada())) || ((*it)->get_nombre() == "tierraDefault")){
					(*it)->setColor(true,x,y, mapa, (personaje -> getXAnclajeNiebla()), (personaje -> getYAnclajeNiebla()), personaje);
					(*it)->blit(pantalla, &cam, NULL,x, y,false);
					(*it)->setDibujada(true, mapa,personaje);
				}else{
					(*it)->blit(pantalla, &cam, NULL,x, y,false);
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
	if(force_no_caminable) {
		return false;
	}else{
		for (auto it = entidades.begin(); it != entidades.end(); ++it){
			if((*it)->isCaminable(this) == false){
				return false;
			}
		}
		return true;
	}
}

void Tile::setNoCaminable() {
	force_no_caminable = true;
}

void Tile::setCaminable() {
	force_no_caminable = false;
}