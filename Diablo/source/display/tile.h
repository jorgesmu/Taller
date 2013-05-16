#pragma once

#include "entidad.h"
#include "../utilities/Personaje.h"
#include <vector>
#include <algorithm>
#include "SDL.h"
class Mapa;
class Personaje;
class Tile {
	private:
	std::vector<Entidad*> entidades;
	int x, y; // x,y son las coordenadas en pixels en la pantalla absoluta
	int u, v; // u,v son las coordenadas logicas del tile

	public:

	// Constantes para los tiles
	static const int TILE_ANCHO = 100;
	static const int TILE_ALTO = 50;
		
	// Ctor, no hace nada
	Tile();
	// deprecated
	void addEntidad(Entidad* ent);
	void addEntidad(Entidad* ent , Mapa* mapa);
	void deleteEntidad(Entidad* ent);
	std::vector<Entidad*> getEntidades();
	void setBlitCoords(int x, int y);
	void setLogicCoords(int u, int v);
	// Blittea todas las entidades en este tile (en el orden que estan en el vector)
	void blit(SDL_Surface* pantalla, Camara& cam, Personaje* personaje, Mapa* mapa);
	// Libera recursos si hace falta
	void clean();
	
	int getX();

	int getY();

	int getU();

	int getV();
	// Devuelve true si el tile no tiene ninguna entidad
	bool sinEntidades() const;

	bool isCaminable();
};