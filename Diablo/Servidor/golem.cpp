#include "golem.h"
const int radioAtaque = 4;
bool Golem::personaje_en_radio(MapaServidor& mapa,PlayerManager& pm){
	//it-> first = nick
	//it-> second = pje
	//TileServidor* tileEnemigo = mapa.getTilePorPixeles(this->getX(),this->getY());
	//int XlogicoEnemigo = tileEnemigo->getU();
	//int YlogicoEnemigo = tileEnemigo->getV();
	int XlogicoEnemigo = this->getX();
	int YlogicoEnemigo = this->getY();

	bool encontro_personaje = false;
	//posiciones fisicas
	for(auto it = pm.getPlayers().begin();it != pm.getPlayers().end(); it++) {
	
		int XLogicoPersonaje = it->second.getX();
		int YLogicoPersonaje = it->second.getY();
		if(strcmp(it->first.c_str(),this->duenio.c_str()) != 0){
			if( (XLogicoPersonaje >= (XlogicoEnemigo - radioAtaque)) && (XLogicoPersonaje <= (XlogicoEnemigo + radioAtaque)) ){
				if( (YLogicoPersonaje >= (YlogicoEnemigo - radioAtaque)) && (YLogicoPersonaje <= (YlogicoEnemigo + radioAtaque)) ){
					encontro_personaje = true;
					break;
				}
			}
		}
	}
	
	return encontro_personaje;
}
