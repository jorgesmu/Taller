#include "mapaservidor.h"

// Ctor
MapaServidor::MapaServidor() {
	w = h = 0;
}

// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
void MapaServidor::resize(int w, int h) {
	this->w = w;
	this->h = h;
	tiles.resize(w*h);
	//se agrega para tener X e Y para armar el camino minimo
	for (int ancho = 0;ancho<w;ancho++){
		for (int alto = 0; alto<h;alto++){
			TileServidor* unTile = this->getTile(ancho,alto);
			unTile->set_x(ancho);
			unTile->set_y(alto);
		}	
	}
}

// Getter para cada tile
TileServidor* MapaServidor::getTile(int x, int y) {
	return &(tiles[x*h + y]);
}

// Chequea que el tile exista
bool MapaServidor::tileExists(int x, int y) const {
	return x >= 0 && y >= 0 && x < w && y < h;
}

// Getter para el vector de tiles, para recorrerlo entero
TileVecServidor& MapaServidor::allTiles() {
	return tiles;
}

void MapaServidor::getSize(int* w, int* h) const {
	*w = this->w;
	*h = this->h;
}

vector <pair<int,int> > MapaServidor::getCaminoMinimo(TileServidor* tileOrigen, TileServidor* tileDestino){
	if(tileOrigen == tileDestino){
		std::vector<pair<int, int>> vacio;
		return vacio;
	}
	
	if(!tileDestino->isCaminable()){
		std::vector<pair<int, int>> vacio;
		return vacio;
	}
	
	int tileOrigenX = tileOrigen->get_x();
	int tileOrigenY = tileOrigen->get_y();
	int tileDestinoX = tileDestino->get_x();
	int tileDestinoY = tileDestino->get_y();
	vector <pair<int,int>> camino = grafoMapa.camino(tileOrigenX,tileOrigenY,tileDestinoX,tileDestinoY);
		
	return camino;
}
//carga el grafo con la informacion del mapa en ese momento
void MapaServidor::cargarGrafo(){
	int altoMapa = this->h;
	int anchoMapa = this->w;
	double raizDeDos = 1.4142;
	double pesoNoCaminable = 500;

	this->grafoMapa = grafo(altoMapa,anchoMapa);

	for(int tileX = 0; tileX < this->w;tileX++){
		for(int tileY = 0; tileY < this->h;tileY++){

			if((tileX-1)>=0){
				//izquierda
				TileServidor* tileArista = getTile(tileX-1,tileY);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX-1,tileY,1);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX-1,tileY,pesoNoCaminable);								
				}
			}
			if((tileY-1)>=0){
				//arriba
				TileServidor* tileArista = getTile(tileX,tileY-1);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX,tileY-1,1);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX,tileY-1,pesoNoCaminable);								
				}
			}
			if((tileX+1)<=anchoMapa-1){
				//derecha
				TileServidor* tileArista = getTile(tileX+1,tileY);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX+1,tileY,1);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX+1,tileY,pesoNoCaminable);								
				}
			}
			if((tileY+1)<=altoMapa-1){
				//abajo
				TileServidor* tileArista = getTile(tileX,tileY+1);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX,tileY+1,1);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX,tileY+1,pesoNoCaminable);								
				}
			}
			if(((tileX-1)>=0) && ((tileY-1)>=0)){
				//arriba izquierda
				TileServidor* tileArista = getTile(tileX-1,tileY-1);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX-1,tileY-1,raizDeDos);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX-1,tileY-1,pesoNoCaminable*raizDeDos);								
				}
			}
			if(((tileX-1)>=0) && ((tileY+1)<=altoMapa-1)){
				//abajo izquierda
				TileServidor* tileArista = getTile(tileX-1,tileY+1);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX-1,tileY+1,raizDeDos);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX-1,tileY+1,pesoNoCaminable*raizDeDos);								
				}
			}
			if(((tileX+1)<=anchoMapa-1) && ((tileY-1)>=0)){
				//arriba derecha
				TileServidor* tileArista = getTile(tileX+1,tileY-1);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX+1,tileY-1,raizDeDos);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX+1,tileY-1,pesoNoCaminable*raizDeDos);								
				}
			}
			if(((tileX+1)<=anchoMapa-1) && ((tileY+1)<=altoMapa-1)){
				//abajo derecha
				TileServidor* tileArista = getTile(tileX+1,tileY+1);
				if(tileArista->isCaminable()){
					grafoMapa.agregar_arista(tileX,tileY,tileX+1,tileY+1,raizDeDos);			
				}else{
					grafoMapa.agregar_arista(tileX,tileY,tileX+1,tileY+1,pesoNoCaminable*raizDeDos);								
				}
			}
		}
	}
}