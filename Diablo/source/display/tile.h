#pragma once

#include "entidad.h"
#include <vector>
#include "SDL.h"

class Tile {
	private:
	std::vector<Entidad> entidades;
	public:
	// Ctor, no hace nada
	Tile();
	// Agrega la entidad al Tile
	void addEntidad(const Entidad& ent);
	// Blittea todas las entidades en este tile (en el orden que estan en el vector)
	void blit();
};