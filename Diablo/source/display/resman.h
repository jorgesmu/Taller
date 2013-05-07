#pragma once

#include "../utilities/surface.h"
#include "../utilities/imagen_error.h"
#include "../display/font.h"
#include <string>
#include <map>
#include <array>
#include "SDL.h"

class ResMan {
	private:
	std::map<std::string, Surface*> surface_map; // Hash table de surfaces
	Surface* err_surface; // Surface especial para error
	// Los tres tamaños basicos de fonts
	// 0 = small, 1 = normal, 2 = big
	std::array<Font*, 3> fonts;

	public:
	// Ctor
	ResMan();
	// Dtor
	~ResMan();
	// Carga la surface de error y los fonts
	bool init(); 
	// Agrega el recurso con nombre name ubicado en el path dado por path
	// Devuelve true o false y loggea en caso de error
	// La primera version es sin color key, la segunda con
	bool addRes(const std::string& name, const std::string& path);
	bool addRes(const std::string& name, const std::string& path, const int colorKey);
	// Devuelve un puntero a la Surface asociada con name
	// En caso de error devuelve la surface de error
	Surface* getRes(const std::string& name) const;
	// Getters para los fonts - por default trae el tamaño NORMAL
	Font* getFont(int size=Font::SIZE_NORMAL) const;
	// Libera todas las surfaces - se tiene que llamar antes de salir
	void clean();

};