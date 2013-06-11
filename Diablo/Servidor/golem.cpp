#include "golem.h"
#include "mapaservidor.h"
const int radioAtaque = 4;
Golem::Golem(const std::string& nickname,const std::string& tipo,unsigned int estrategiaElegida,string nickDuenio){
	this->init_Enemy(nickname, tipo,estrategiaElegida); 
	this->duenio = nickDuenio;
}

bool Golem::personaje_en_radio(MapaServidor& mapa,PlayerManager& pm,TileServidor*& tilePersonaje){
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
		if(it->second.getNick() != this->get_duenio()){
			if( (XLogicoPersonaje >= (XlogicoEnemigo - radioAtaque)) && (XLogicoPersonaje <= (XlogicoEnemigo + radioAtaque)) ){
				if( (YLogicoPersonaje >= (YlogicoEnemigo - radioAtaque)) && (YLogicoPersonaje <= (YlogicoEnemigo + radioAtaque)) ){
					encontro_personaje = true;
					tilePersonaje = mapa.getTile( XLogicoPersonaje,YLogicoPersonaje);
					break;
				}
			}
		}
	}
	if(!encontro_personaje){
		for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end(); it++) {
	
			int XLogicoPersonaje = it->second->getX();
			int YLogicoPersonaje = it->second->getY();
			if(it->second->getNick() != this->get_duenio()){
				if( (XLogicoPersonaje >= (XlogicoEnemigo - radioAtaque)) && (XLogicoPersonaje <= (XlogicoEnemigo + radioAtaque)) ){
					if( (YLogicoPersonaje >= (YlogicoEnemigo - radioAtaque)) && (YLogicoPersonaje <= (YlogicoEnemigo + radioAtaque)) ){
						encontro_personaje = true;
						tilePersonaje = mapa.getTile( XLogicoPersonaje,YLogicoPersonaje);
						break;
					}
				}
			}
		}
	
	}
	return encontro_personaje;
}
bool Golem::personaje_adyacente(MapaServidor& mapa,PlayerManager& pm, TileServidor*& tilePersonaje,string& enemigoAtacado){
	bool res = false;
	for(auto it = pm.getPlayers().begin();it != pm.getPlayers().end();it++) {
		int x = it->second.getX() - this->getX();
		int y = it->second.getY() - this->getY();
		if(abs(x) <=1 && abs(y)<=1){
			if(it->second.getNick() != this->get_duenio()){
				tilePersonaje = mapa.getTile(it->second.getX(),it->second.getY());
				res = true;
				enemigoAtacado = it->second.getNick();
				break;
			}
		}
	}
	for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end();it++) {
		int x = it->second->getX() - this->getX();
		int y = it->second->getY() - this->getY();
		if(abs(x) <=1 && abs(y)<=1){
			if(it->second->getNick() != this->get_duenio()){
				tilePersonaje = mapa.getTile(it->second->getX(),it->second->getY());
				res = true;
				enemigoAtacado = it->second->getNick();
				break;
			}
		}
	}

	return res;
}
TileServidor* Golem::get_proximo_tile_enemigo(MapaServidor& mapa,PlayerManager& pm){
	TileServidor* res = NULL;
	TileServidor* tileDestino = NULL;
	bool estaEnRadio = this->personaje_en_radio(mapa,pm,tileDestino);
	if (estaEnRadio){
		//sigo al personaje por el camino minimo

		TileServidor* tileOrigen = mapa.getTile(this->getX(),this->getY());
		vector<pair <int,int> > camino = mapa.getCaminoMinimo(tileOrigen,tileDestino);
		if (camino.size() > 0 )
			res = mapa.getTile(camino[1].first,camino[1].second);
	}else{
		//sigo mi camino segun mi estrategia
		if (this->estrategia == 1){
			res = this->get_proximo_tile_recta_horizontal(mapa);
		}else if(this->estrategia == 2){
			res = this->get_proximo_tile_recta_vertical(mapa);
		}else if(this->estrategia == 3){
			res = this->get_proximo_tile_circular(mapa);
		}
	}
	return res;
}
