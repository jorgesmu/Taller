#include "mapa.h"
#include "camara.h"
#include "../utilities/vec2.h"
#include "../utilities/coordenadas.h"
#include "../utilities/grafo.h"
#include "../net/PjeManager.h"

// Ctor
Mapa::Mapa() {
	w = h = 0;
}

// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
void Mapa::resize(int w, int h) {
	this->w = w;
	this->h = h;
	tiles.resize(w*h);
	assignTileCoords();
}

// Getter para cada tile
Tile* Mapa::getTile(int x, int y) {
	return &(tiles[x*h + y]);
}

// Chequea que el tile exista
bool Mapa::tileExists(int x, int y) const {
	return x >= 0 && y >= 0 && x < w && y < h;
}

// Getter para el vector de tiles, para recorrerlo entero
TileVec& Mapa::allTiles() {
	return tiles;
}

void Mapa::blit(SDL_Surface* dest, Camara& camara, Personaje* personaje) {
	// Primero calculamos cuantos tiles nos vamos a tener que mover horizontal y verticalmente
	const int tile_count_x = (camara.getW() / Tile::TILE_ANCHO) + 5;
	const int tile_count_y = 2*((camara.getH() / Tile::TILE_ALTO) + 5);
	// Calculamos el tile de la esquina superior izquierda
	vec2<int> curr_tile = MouseCoords2Tile(vec2<int>(0, 0), camara);
	// Lo movemos uno mas para el N y al E
	curr_tile = tileWalk(curr_tile, GDIR::E);
	curr_tile = tileWalk(curr_tile, GDIR::E);
	curr_tile = tileWalk(curr_tile, GDIR::NE);
	curr_tile = tileWalk(curr_tile, GDIR::N);
	// Con esta variable controlamos si cuando bajamos de fila vamos hacia el SO o hacia el el SE
	bool last_turn_SO = true;
	// Iteramos
	for(int y = 0;y < tile_count_y;y++) {
		vec2<int> fila_start = curr_tile;
		for(int x = 0;x < tile_count_x;x++) {
			// Si el tile en el que estamos parados es valido, lo blitteamos
			if(tileExists(curr_tile.x, curr_tile.y)) {
				getTile(curr_tile.x, curr_tile.y)->blit(dest, camara, personaje, this);
			}
			// Nos movemos para el oeste
			curr_tile = tileWalk(curr_tile, GDIR::O);
		}
		// Bajamos de fila
		if(last_turn_SO) {
			curr_tile = tileWalk(fila_start, GDIR::SE);
		}else{
			curr_tile = tileWalk(fila_start, GDIR::SO);
		}
		last_turn_SO = !last_turn_SO;
	}


	// Version que blittea recorriendo los tiles matricialmente
	//for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		//it->blit(dest, camara);
	//}
}

void Mapa::clean() {
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		it->clean();
	}
}


void Mapa::setEntidadesDibujadasFalse(){
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		std::vector<Entidad*> entidadesTile = it->getEntidades();
		for(auto it2 = entidadesTile.begin(); it2 != entidadesTile.end(); ++it2){
			(*it2)->setDibujada(false);
		}
	}
}


void Mapa::assignTileCoords() {
	int x, y;
	for(x = 0;x < w;x++) {
		for(y = 0;y < h;y++) {
			Tile* tile = getTile(x,y);
			tile->setBlitCoords((x-y)*Tile::TILE_ANCHO/2, (x+y)*Tile::TILE_ALTO/2);
			tile->setLogicCoords(x,y);
		}
	}
}

// Devuelve el tile segun su posicion en el mapa dado por el pixel superior
Tile* Mapa::getTilePorPixeles(int pixelX , int pixelY) {
	Tile* retorno = NULL;
	int x = 0;
	int y = 0;
	int x0 = pixelX ;
	int y0 = pixelY - Tile::TILE_ALTO/2;
	x = y0 + ((x0 + Tile::TILE_ANCHO/2) / 2);
	y = y0 - ((x0 - Tile::TILE_ANCHO/2) / 2);
	x /= Tile::TILE_ALTO;
	y /= Tile::TILE_ALTO;
	if ((x < this -> w) && (x >= 0) && 
		(y < this -> h) && (y >= 0)){
		retorno = this -> getTile(x , y);
	}
	return retorno;
}

void Mapa::getSize(int* w, int* h) const {
	*w = this->w;
	*h = this->h;
}

vector <pair<int,int> > Mapa::getCaminoMinimo(Tile* tileOrigen, Tile* tileDestino){
	if(tileOrigen == tileDestino){
		std::vector<pair<int, int>> vacio;
		return vacio;
	}

	int altoMapa = this->h;
	int anchoMapa = this->w;
	double raizDeDos = 1.4142;
	double pesoNoCaminable = 500;
	grafo miGrafo(anchoMapa,altoMapa);

	//std::map<std::string, Personaje>& personajesDelMapa = pjm.getPjes();

	for (auto it= allTiles().begin(); it != allTiles().end(); ++it){
		int tileX = it->getU();
		int tileY = it->getV();

		if((tileX-1)>=0){
			//izquierda
			Tile* tileArista = getTile(tileX-1,tileY);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX-1,tileY,1);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX-1,tileY,pesoNoCaminable);								
			}
		}
		if((tileY-1)>=0){
			//arriba
			Tile* tileArista = getTile(tileX,tileY-1);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX,tileY-1,1);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX,tileY-1,pesoNoCaminable);								
			}
		}
		if((tileX+1)<=anchoMapa-1){
			//derecha
			Tile* tileArista = getTile(tileX+1,tileY);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX+1,tileY,1);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX+1,tileY,pesoNoCaminable);								
			}
		}
		if((tileY+1)<=altoMapa-1){
			//abajo
			Tile* tileArista = getTile(tileX,tileY+1);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX,tileY+1,1);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX,tileY+1,pesoNoCaminable);								
			}
		}
		if(((tileX-1)>=0) && ((tileY-1)>=0)){
			//arriba izquierda
			Tile* tileArista = getTile(tileX-1,tileY-1);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX-1,tileY-1,raizDeDos);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX-1,tileY-1,pesoNoCaminable*raizDeDos);								
			}
		}
		if(((tileX-1)>=0) && ((tileY+1)<=altoMapa-1)){
			//abajo izquierda
			Tile* tileArista = getTile(tileX-1,tileY+1);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX-1,tileY+1,raizDeDos);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX-1,tileY+1,pesoNoCaminable*raizDeDos);								
			}
		}
		if(((tileX+1)<=anchoMapa-1) && ((tileY-1)>=0)){
			//arriba derecha
			Tile* tileArista = getTile(tileX+1,tileY-1);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX+1,tileY-1,raizDeDos);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX+1,tileY-1,pesoNoCaminable*raizDeDos);								
			}
		}
		if(((tileX+1)<=anchoMapa-1) && ((tileY+1)<=altoMapa-1)){
			//abajo derecha
			Tile* tileArista = getTile(tileX+1,tileY+1);
			if(tileArista->isCaminable()){
				miGrafo.agregar_arista(tileX,tileY,tileX+1,tileY+1,raizDeDos);			
			}else{
				miGrafo.agregar_arista(tileX,tileY,tileX+1,tileY+1,pesoNoCaminable*raizDeDos);								
			}
		}
	}
	int tileOrigenX = tileOrigen->getU();
	int tileOrigenY = tileOrigen->getV();
	int tileDestinoX = tileDestino->getU();
	int tileDestinoY = tileDestino->getV();
	vector <pair<int,int>> camino = miGrafo.camino(tileOrigenX,tileOrigenY,tileDestinoX,tileDestinoY);

	return camino;
}