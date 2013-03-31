#pragma once

#include "entidad.h"
#include <vector>
#include "SDL.h"

class Tile {
	private:
	std::vector<Entidad> entidades;
	int x, y, u, v; //x,y son las coordenadas en pixels mientras que u,v son las coordenadas de la grilla isometrica

	static const int TILE_ANCHO = 53;
	static const int TILE_ALTO = 44;

	public:
	// Ctor, no hace nada
	Tile();

	int getX();	

	int getY();

	int getU();

	int getV();

	int getAlto();

	int getAncho();

	void setX(int x);	

	void setY(int y);

	void setU(int u);

	void setV(int v);

	//void setAlto(int h);

	//void setAncho(int w);

	void addEntidad(const Entidad& ent);

	void deleteEntidad(const Entidad& ent);
	
	// Blittea todas las entidades en este tile (en el orden que estan en el vector)
	void blit(SDL_Surface* pantalla, SDL_Rect destino);
};