#pragma once

#include <vector>

// clase de Tile para el mapa servidor
// por default es caminable
class TileServidor {
	private:
	bool caminable;

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
};

// Mapa servidor
typedef std::vector<TileServidor> TileVecServidor;

class MapaServidor {
	private:
	int w, h;
	TileVecServidor tiles;

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
	// Getter para el vector de tiles, para recorrerlo entero
	TileVecServidor& allTiles();
};