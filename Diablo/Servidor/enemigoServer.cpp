#include "enemigoServer.h"
#include "playerman.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "../source/net/bitstream.h"
#include "../source/net/defines.h"
#include "../source/utilities/aux_func.h"
#include "misiones.h"
#include "golem.h"
const int noSeMovio = 0;
const int derecha = 1;
const int izquierda = 2;
const int arriba = 3;
const int abajo = 4;
const int pasosMax = 2;
const int radioAtaque = 2;// en golem tambien esta definido
void Enemigo::init_Enemy(const std::string& nickname,const std::string& tipo,unsigned int estrategiaElegida){
	this->init(nickname,tipo);
	this->estrategia = estrategiaElegida;
	this->direccion = 0;
	this->pasosCaminados = 0;
	this->ultimaAccionAtacar = false;
	this->setVelocidad(0.105);
	this->setBolaDeCristal(false);
	this->setGolem(false);
	this->setEnergia(100);
	this->setEnergiaEscudo(0);
}
extern Misiones mision;

//getters


TileServidor* Enemigo::get_proximo_tile_recta_horizontal(MapaServidor& Mapa,PlayerManager& pm){
	if (this->direccion == noSeMovio){
		this->direccion = derecha;
	} 

	//TileServidor* tileActual = Mapa.getTile(this->x(),this->y());
	int u = this->getX();
	int v = this->getY();
	bool existeDerecha = Mapa.tileExists(u+1,v);
	bool existeIzquierda = Mapa.tileExists(u-1,v);
	TileServidor* tileDerecha = NULL;
	TileServidor* tileIzquierda = NULL;
	if (existeDerecha){
		tileDerecha = Mapa.getTile(u + 1,v);
	}
	if (existeIzquierda){
		tileIzquierda = Mapa.getTile(u - 1,v);
	}
	if (this->pasosCaminados <= pasosMax){
		//si estoy en mi radio de caminata	
		pasosCaminados++;
		if(this ->direccion == derecha){
			//si estoy yendo para la derecha
			if(existeDerecha){
				if	(tileDerecha->isCaminable() && !Mapa.tile_esta_ocupado(tileDerecha->get_x(),tileDerecha->get_y(),pm)){
					return tileDerecha;
				}else{
					if (existeIzquierda){
						if (tileIzquierda->isCaminable() && !Mapa.tile_esta_ocupado(tileIzquierda->get_x(),tileIzquierda->get_y(),pm)){
							this->pasosCaminados = 0;
							this->direccion = izquierda;
							return tileIzquierda;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA DERECHA
				if (existeIzquierda){
					if (tileIzquierda->isCaminable() && !Mapa.tile_esta_ocupado(tileIzquierda->get_x(),tileIzquierda->get_y(),pm)){
						this->pasosCaminados = 0;
						this->direccion = izquierda;					
						return tileIzquierda;	
					}
				}
			}
		}else {
			//si estoy yendo para la izquierda
			if(existeIzquierda){
				if	(tileIzquierda->isCaminable() && !Mapa.tile_esta_ocupado(tileIzquierda->get_x(),tileIzquierda->get_y(),pm)){
					return tileIzquierda;
				}else{
					if (existeDerecha){
						if (tileDerecha->isCaminable()  && !Mapa.tile_esta_ocupado(tileDerecha->get_x(),tileDerecha->get_y(),pm)){
							this->pasosCaminados = 0;
							this->direccion = derecha;
							return tileDerecha;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA izquierda
				if (existeDerecha){
					if (tileDerecha->isCaminable() && !Mapa.tile_esta_ocupado(tileDerecha->get_x(),tileDerecha->get_y(),pm)){
						this->pasosCaminados = 0;
						this->direccion = derecha;					
						return tileDerecha;	
					}
				}
			}
		}
	}else{
		//cambio de direccion porque temrino los pasos
		this->pasosCaminados = 0 ;
		if (this->direccion == izquierda){
			if (existeDerecha){
				if(tileDerecha->isCaminable() && !Mapa.tile_esta_ocupado(tileDerecha->get_x(),tileDerecha->get_y(),pm)){
					//cambio direccion
					this->direccion = derecha;
					return  tileDerecha;
				}else{
					if(existeIzquierda){
						if (tileIzquierda->isCaminable() && !Mapa.tile_esta_ocupado(tileIzquierda->get_x(),tileIzquierda->get_y(),pm)){
							return tileIzquierda;
						}else{
							return NULL;
						}
					}
				}
			}
		}else{
			//si direccion era derecha
			if (existeIzquierda){
				if(tileIzquierda->isCaminable() && !Mapa.tile_esta_ocupado(tileIzquierda->get_x(),tileIzquierda->get_y(),pm)){
					//cambio direccion
					this->direccion = izquierda;
					return  tileIzquierda;
				}else{
					if(existeDerecha){
						if (tileDerecha->isCaminable() && !Mapa.tile_esta_ocupado(tileDerecha->get_x(),tileDerecha->get_y(),pm)){
							return tileDerecha;
						}else{
							return NULL;
						}
					}
				}
			}
		}
	}
		return NULL;
}
TileServidor* Enemigo::get_proximo_tile_recta_vertical(MapaServidor& Mapa,PlayerManager& pm){
		if (this->direccion == noSeMovio){
			this->direccion = arriba;
	} 

	//TileServidor* tileActual = Mapa.getTilePorPixeles(this->getX(),this->getY());
	//int u = tileActual->getU();
	//int v = tileActual->getV();
	int u =this->getX();
	int v= this->getY();
	bool existeArriba = Mapa.tileExists(u,v + 1);
	bool existeAbajo = Mapa.tileExists(u,v-1);
	TileServidor* tileArriba = NULL;
	TileServidor* tileAbajo = NULL;
	if (existeArriba){
		tileArriba = Mapa.getTile(u,v+1);
	}
	if (existeAbajo){
		tileAbajo = Mapa.getTile(u,v-1);
	}
	if (this->pasosCaminados <= pasosMax){
		//si estoy en mi radio de caminata	
		pasosCaminados++;
		if(this ->direccion == arriba){
			//si estoy yendo para la arriba
			if(existeArriba){
				if	(tileArriba->isCaminable() && !Mapa.tile_esta_ocupado(tileArriba->get_x(),tileArriba->get_y(),pm)){
					return tileArriba;
				}else{
					if (existeAbajo){
						if (tileAbajo->isCaminable() && !Mapa.tile_esta_ocupado(tileAbajo->get_x(),tileAbajo->get_y(),pm)){
							this->pasosCaminados = 0;
							this->direccion = abajo;
							return tileAbajo;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA arriba
				if (existeAbajo){
					if (tileAbajo->isCaminable() && !Mapa.tile_esta_ocupado(tileAbajo->get_x(),tileAbajo->get_y(),pm)){
						this->pasosCaminados = 0;
						this->direccion = abajo;					
						return tileAbajo;	
					}
				}
			}
		}else {
			//si estoy yendo para la abajo
			if(existeAbajo){
				if	(tileAbajo->isCaminable() && !Mapa.tile_esta_ocupado(tileAbajo->get_x(),tileAbajo->get_y(),pm)){
					return tileAbajo;
				}else{
					if (existeArriba){
						if (tileArriba->isCaminable()  && !Mapa.tile_esta_ocupado(tileArriba->get_x(),tileArriba->get_y(),pm)){
							this->pasosCaminados = 0;
							this->direccion = arriba;
							return tileArriba;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA abajo
				if (existeArriba){
					if (tileArriba->isCaminable() && !Mapa.tile_esta_ocupado(tileArriba->get_x(),tileArriba->get_y(),pm)){
						this->pasosCaminados = 0;
						this->direccion = arriba;					
						return tileArriba;	
					}
				}
			}
		}
	}else{
		//cambio de direccion porque temrino los pasos
		this->pasosCaminados = 0 ;
		if (this->direccion == abajo){
			if (existeArriba){
				if(tileArriba->isCaminable() && !Mapa.tile_esta_ocupado(tileArriba->get_x(),tileArriba->get_y(),pm)){
					//cambio direccion
					this->direccion = arriba;
					return  tileArriba;
				}else{
					if(existeAbajo){
						if (tileAbajo->isCaminable() && !Mapa.tile_esta_ocupado(tileAbajo->get_x(),tileAbajo->get_y(),pm)){
							return tileAbajo;
						}else{
							return NULL;
						}
					}
				}
			}
		}else{
			//si direccion era arriba
			if (existeAbajo){
				if(tileAbajo->isCaminable() && !Mapa.tile_esta_ocupado(tileAbajo->get_x(),tileAbajo->get_y(),pm)){
					//cambio direccion
					this->direccion = abajo;
					return  tileAbajo;
				}else{
					if(existeArriba){
						if (tileArriba->isCaminable() && !Mapa.tile_esta_ocupado(tileArriba->get_x(),tileArriba->get_y(),pm)){
							return tileArriba;
						}else{
							return NULL;
						}
					}
				}
			}
		}
	}
		return NULL;
}
TileServidor * Enemigo::get_proximo_tile_circular(MapaServidor& Mapa,PlayerManager& pm){
	TileServidor* res = NULL;
	if (this->direccion == noSeMovio){
		this->direccion = derecha;
	}
	if (this->pasosCaminados >= pasosMax){
		if (this->direccion == derecha){
			this->direccion = abajo;
			res = get_proximo_tile_recta_vertical(Mapa,pm);
		}else if (this->direccion == abajo){
			this->direccion = izquierda;
			res = get_proximo_tile_recta_horizontal(Mapa,pm);
		}else if (this->direccion == izquierda){
			this->direccion = arriba;
			res = get_proximo_tile_recta_vertical(Mapa,pm);
		}else if (this->direccion == arriba){
			this->direccion = derecha;
			res = get_proximo_tile_recta_horizontal(Mapa,pm);
		}
		this->pasosCaminados = 0;
	}else{
		//camino en la dir que estaba
		if (this->direccion == arriba || this->direccion == abajo){
			res = get_proximo_tile_recta_vertical(Mapa,pm);
		}else{
			res = get_proximo_tile_recta_horizontal(Mapa,pm);
		}
	}

	return res;
	
}
bool Enemigo::personaje_en_radio(MapaServidor& mapa,PlayerManager& pm,TileServidor*& tilePersonaje){
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

		if( (XLogicoPersonaje >= (XlogicoEnemigo - radioAtaque)) && (XLogicoPersonaje <= (XlogicoEnemigo + radioAtaque)) ){
			if( (YLogicoPersonaje >= (YlogicoEnemigo - radioAtaque)) && (YLogicoPersonaje <= (YlogicoEnemigo + radioAtaque)) ){
				encontro_personaje = true;
				tilePersonaje = mapa.getTile( XLogicoPersonaje,YLogicoPersonaje);
				break;
			}
		}
	}
	
	return encontro_personaje;
}
TileServidor* Enemigo::get_proximo_tile_enemigo(MapaServidor& mapa,PlayerManager& pm){
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
			res = this->get_proximo_tile_recta_horizontal(mapa,pm);
		}else if(this->estrategia == 2){
			res = this->get_proximo_tile_recta_vertical(mapa,pm);
		}else if(this->estrategia == 3){
			res = this->get_proximo_tile_circular(mapa,pm);
		}
	}
	return res;
}
bool Enemigo::personaje_adyacente(MapaServidor& mapa,PlayerManager& pm, TileServidor*& tilePersonaje,string& enemigoAtacado){
	bool res = false;
	for(auto it = pm.getPlayers().begin();it != pm.getPlayers().end();it++) {
		int x = it->second.getX() - this->getX();
		int y = it->second.getY() - this->getY();
		if(abs(x) <=1 && abs(y)<=1){
			tilePersonaje = mapa.getTile(it->second.getX(),it->second.getY());
			res = true;
			enemigoAtacado = it->second.getNick();
			break;
		}
	}
	return res;
}
void Enemigo::atacar(string& NickAtacado,PlayerManager& pm,ServerSocket& socks){
	BitStream bs;
	int danio = intRand(0,30);
	//resto danio si ataca a un golem o a un enemigo
	bool encontro = false;
	bool murio = false;
	for(auto it = pm.getEnemies().begin();it !=pm.getEnemies().end();it++) {
		Enemigo* unEnemigo = it->second;
		if(unEnemigo->getNick() == NickAtacado){
			unEnemigo->hacerDanio(danio);
			encontro = true;
			if(!it->second->estaVivo()){
				murio = true;
				pm.getEnemies().erase(it);
				break;
			}
		}
	}
	//para golem
	if(!encontro){
		for(auto it = pm.getGolems().begin();it !=pm.getGolems().end();it++) {
			Golem* unGolem = it->second;
			if(unGolem->getNick() == NickAtacado){
				unGolem->hacerDanio(danio);
			
				if(!it->second->estaVivo()){
					//aviso a los demas que murio enemigo
					murio = true;
					pm.getGolems().erase(it);
					break;
				}
			}
		}
	}
	//aviso del ataque
	for(auto it = socks.get_clients().begin();it !=socks.get_clients().end();it++) {
		//ataco con la danio
		bs.clear();
		bs << PROTO::ATACAR << this->getNick() << NickAtacado;
		it->second.send(bs.str());
		//mando danio
		bs.clear();
		bs << PROTO::DAMAGE << this->getNick() << NickAtacado << danio;
		it->second.send(bs.str());
		std::cout << "Update " << it->second.nick << " que " << this->getNick() << "->" << NickAtacado << endl;
		//aviso si murio
		if(murio){
			//Veo si termino la mision
			if (mision.getTipo() == Misiones::MISION_ENEMIGO) {
				if (mision.enemigoMision() == NickAtacado) {
					bs.clear();
					bs << PROTO::WINNER << pm.getEnemy(NickAtacado)->ultimoAtacante();
					it->second.send(bs.str());
				}
			} else {
				bs.clear();
				bs << PROTO::ENEMY_DEAD << NickAtacado;
				it->second.send(bs.str());
			}
		}
	}
	return;
}
