#include "tile.h"
#include "mapa.h"

Tile::Tile() {
	force_no_caminable = false;
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

void Tile::setearExplorados(int tileX, int tileY, Personaje* personaje, Mapa* mapa){

	Tile* tile = mapa->getTile(tileX, tileY);
	for(int posX = (Tile::TILE_ANCHO/2) + tile->getX()-(Personaje::RADIO_VISION_X); posX < tile->getX()+(Personaje::RADIO_VISION_X); posX+=Tile::TILE_ANCHO/2){
		for(int posY = (Tile::TILE_ANCHO/2) + tile->getY()-(Personaje::RADIO_VISION_Y); posY < tile->getY()+(Personaje::RADIO_VISION_Y); posY+=Tile::TILE_ALTO/2){
			int deltaX = tile->getX() - posX;
			int deltaY = tile->getY() - posY;

			if((abs(deltaX) <= Personaje::RADIO_VISION_X) && (abs(deltaY) <= Personaje::RADIO_VISION_Y)){
				personaje->agregarTilesExplorados(mapa->getTilePorPixeles(posX, posY));
			}

		}
	}
}

void Tile::blit(SDL_Surface* pantalla, Camara& cam, Personaje* personaje, Mapa* mapa){

	Tile* tilePersonaje = mapa->getTilePorPixeles(personaje->getX(), personaje->getY());
	int deltaX = personaje -> getXAnclajeNiebla() - x;
	int deltaY = personaje -> getYAnclajeNiebla() - y;
	if((abs(deltaX) <= Personaje::RADIO_VISION_X) && (abs(deltaY) <= Personaje::RADIO_VISION_Y)){
		//dibujo en colores normales
		for(auto it = entidades.begin(); it != entidades.end(); ++it) {
			if ( (!(*it)-> isInTile(this->x , this -> y)) && (!(*it)->isCompartido())){
				// es un ancla
				deltaX = personaje -> getXAnclajeNiebla() - (*it)->getX();
				deltaY = personaje -> getYAnclajeNiebla() - (*it)->getY();
				if((abs(deltaX) <= Personaje::RADIO_VISION_X) && (abs(deltaY) <= Personaje::RADIO_VISION_Y)){
					(*it)->setColor(true,x,y, mapa, (personaje -> getXAnclajeNiebla()), (personaje -> getYAnclajeNiebla()));
					(*it)->blit(pantalla, &cam, NULL,x, y,true);
					(*it)->setDibujada(true, mapa,personaje);				
				}
			}else{
				(*it)->setColor(true,x,y, mapa, (personaje -> getXAnclajeNiebla()), (personaje -> getYAnclajeNiebla()));
				(*it)->blit(pantalla, &cam, NULL,x, y,true);
				(*it)->setDibujada(true, mapa,personaje);			
			}
		}		
		personaje->agregarTilesExplorados(this);
	}else{
		std::vector<Tile*> tilesExplorados = personaje->getTilesExplorados();

		if(std::find(tilesExplorados.begin(), tilesExplorados.end(), this) != tilesExplorados.end()){
			//lo bliteo en gris
			for(auto it = entidades.begin(); it != entidades.end(); ++it) {
				if((*it)->noDibujaFueraDelRadio()) continue;

				if((!((*it)->getDibujada())) || ((*it)->get_nombre() == "tierraDefault")){
					(*it)->setColor(true,x,y, mapa, (personaje -> getXAnclajeNiebla()), (personaje -> getYAnclajeNiebla()));
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