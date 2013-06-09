#pragma once

#include <vector>
#include <algorithm>
#include "../source/utilities/grafo.h"
// clase de Tile para el mapa servidor
// por default es caminable
using namespace std;
class TileServidor {
	private:
	bool caminable;
	int x,y;

	public:
	TileServidor() {
		setCaminable();
	}
	bool isCaminable() const {
		return caminable;
	}
	void setCaminable() {
		caminable = true;
	}
	void setNoCaminable() {
		caminable = false;
	}
	//gets
	int get_x(){
		return x;
	}
	int get_y(){
		return y;
	}
	//sets
	void set_x(int new_value){
		x = new_value;
	}
	void set_y(int new_value){
		y = new_value;
	}
};

// Mapa servidor
typedef std::vector<TileServidor> TileVecServidor;

class MapaServidor {
	private:
	int w, h;
	TileVecServidor tiles;
	grafo grafoMapa;
	public:
	MapaServidor();
	// Resize para el mapa
	void resize(int w, int h);
	// Getter para tamaño
	void getSize(int* w, int* h) const;
	// Getter para cada tile
	TileServidor* getTile(int x, int y);
	// Verifica si el tile existe
	bool tileExists(int x, int y) const;
	//devuelve el camino minimo
	vector<pair<int,int> > getCaminoMinimo(TileServidor* tileOrigen, TileServidor* tileDestino);
	//vector <pair<int,int>> getCaminoMinimo(TileServidor* tileOrigen, TileServidor* tileDestino);
	// Getter para el vector de tiles, para recorrerlo entero
	TileVecServidor& allTiles();
	//cargar grafo
	void cargarGrafo();
};