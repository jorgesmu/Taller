#include "enemigoServer.h"
#include "playerman.h"
#include <vector>
#include <algorithm>

const int noSeMovio = 0;
const int derecha = 1;
const int izquierda = 2;
const int arriba = 3;
const int abajo = 4;
const int pasosMax = 2;
const int radioAtaque = 4;// en golem tambien esta definido
void Enemigo::init_Enemy(const std::string& nickname,const std::string& tipo,unsigned int estrategiaElegida){
	this->init(nickname,tipo);
	this->estrategia = estrategiaElegida;
	this->direccion = 0;
	this->pasosCaminados = 0;
}

//getters


TileServidor* Enemigo::get_proximo_tile_recta_horizontal(MapaServidor& Mapa){
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
				if	(tileDerecha->isCaminable()){
					return tileDerecha;
				}else{
					if (existeIzquierda){
						if (tileIzquierda->isCaminable()){
							this->pasosCaminados = 0;
							this->direccion = izquierda;
							return tileIzquierda;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA DERECHA
				if (existeIzquierda){
					if (tileIzquierda->isCaminable()){
						this->pasosCaminados = 0;
						this->direccion = izquierda;					
						return tileIzquierda;	
					}
				}
			}
		}else {
			//si estoy yendo para la izquierda
			if(existeIzquierda){
				if	(tileIzquierda->isCaminable()){
					return tileIzquierda;
				}else{
					if (existeDerecha){
						if (tileDerecha->isCaminable()){
							this->pasosCaminados = 0;
							this->direccion = derecha;
							return tileDerecha;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA izquierda
				if (existeDerecha){
					if (tileDerecha->isCaminable()){
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
				if(tileDerecha->isCaminable()){
					//cambio direccion
					this->direccion = derecha;
					return  tileDerecha;
				}else{
					if(existeIzquierda){
						if (tileIzquierda->isCaminable()){
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
				if(tileIzquierda->isCaminable()){
					//cambio direccion
					this->direccion = izquierda;
					return  tileIzquierda;
				}else{
					if(existeDerecha){
						if (tileDerecha->isCaminable()){
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
TileServidor* Enemigo::get_proximo_tile_recta_vertical(MapaServidor& Mapa){
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
				if	(tileArriba->isCaminable()){
					return tileArriba;
				}else{
					if (existeAbajo){
						if (tileAbajo->isCaminable()){
							this->pasosCaminados = 0;
							this->direccion = abajo;
							return tileAbajo;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA arriba
				if (existeAbajo){
					if (tileAbajo->isCaminable()){
						this->pasosCaminados = 0;
						this->direccion = abajo;					
						return tileAbajo;	
					}
				}
			}
		}else {
			//si estoy yendo para la abajo
			if(existeAbajo){
				if	(tileAbajo->isCaminable()){
					return tileAbajo;
				}else{
					if (existeArriba){
						if (tileArriba->isCaminable()){
							this->pasosCaminados = 0;
							this->direccion = arriba;
							return tileArriba;	
						}
					}
				}
			}else{
				//NO HAY MAS TILES A LA abajo
				if (existeArriba){
					if (tileArriba->isCaminable()){
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
				if(tileArriba->isCaminable()){
					//cambio direccion
					this->direccion = arriba;
					return  tileArriba;
				}else{
					if(existeAbajo){
						if (tileAbajo->isCaminable()){
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
				if(tileAbajo->isCaminable()){
					//cambio direccion
					this->direccion = abajo;
					return  tileAbajo;
				}else{
					if(existeArriba){
						if (tileArriba->isCaminable()){
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
TileServidor * Enemigo::get_proximo_tile_circular(MapaServidor& Mapa){
	TileServidor* res = NULL;
	if (this->direccion == noSeMovio){
		this->direccion = derecha;
	}
	if (this->pasosCaminados >= pasosMax){
		if (this->direccion == derecha){
			this->direccion = abajo;
			res = get_proximo_tile_recta_vertical(Mapa);
		}else if (this->direccion == abajo){
			this->direccion = izquierda;
			res = get_proximo_tile_recta_horizontal(Mapa);
		}else if (this->direccion == izquierda){
			this->direccion = arriba;
			res = get_proximo_tile_recta_vertical(Mapa);
		}else if (this->direccion == arriba){
			this->direccion = derecha;
			res = get_proximo_tile_recta_horizontal(Mapa);
		}
		this->pasosCaminados = 0;
	}else{
		//camino en la dir que estaba
		if (this->direccion == arriba || this->direccion == abajo){
			res = get_proximo_tile_recta_vertical(Mapa);
		}else{
			res = get_proximo_tile_recta_horizontal(Mapa);
		}
	}

	return res;
	
}
bool Enemigo::personaje_en_radio(MapaServidor& mapa,PlayerManager& pm){
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
				break;
			}
		}
	}
	
	return encontro_personaje;
}
TileServidor* Enemigo::get_proximo_tile_enemigo(MapaServidor& mapa,PlayerManager& pm){
	TileServidor* res = NULL;
	bool estaEnRadio = this->personaje_en_radio(mapa,pm);
	if (estaEnRadio){
		//sigo al personaje por el camino minimo
	//	TileServidor* tileOrigen = mapa.getTilePorPixeles(this->getX(),this->getY());
	//	tileOrigen* tileDestino = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(),pjm.getPjeLocal().getY());
		//vector<pair <int,int> > camino = mapa.getCaminoMinimo(tileOrigen,tileDestino);
	//	if (camino.size() > 0 )
		//	res = mapa.getTile(camino[1].first,camino[1].second);
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