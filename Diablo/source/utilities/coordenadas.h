#pragma once

#include "vec2.h"
#include "../display/tile.h"
#include "../display/camara.h"

#include <iostream>

// Funcion que mappea posicion del mouse (+scroll) a grilla de tiles
template <typename T>
vec2<T> MouseCoords2Tile(const vec2<T>& mouse_coords, const Camara& cam) {
	vec2<T> grid;
	T x0 = (T)cam.getX() + mouse_coords.x;
	x0 -= Tile::TILE_ANCHO/2;
	T y0 = (T)cam.getY() + mouse_coords.y;
	grid.x = y0 + (x0 / 2);
	grid.y = y0 - (x0 / 2);
	grid.x /= Tile::TILE_ALTO;
	grid.y /= Tile::TILE_ALTO;
	return grid;
}

// Funcion para movernos en la grilla logica
// Primero definimos las direcciones
namespace GDIR {
	enum {
		N = 0,
		NO,
		O,
		SO,
		S,
		SE,
		E,
		NE
	};
}
// La funcion en cuestion
vec2<int> tileWalk(const vec2<int>& current, int dir);