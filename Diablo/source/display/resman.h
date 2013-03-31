#pragma once

#include "../utilities/surface.h"
#include <string>
#include <map>
#include "SDL.h"

class ResMan {
	private:
	std::map<std::string, Surface*> surface_map;

	public:
	// Ctor
	ResMan();
	// Dtor
	~ResMan();
	// Carga la surface de error 
	void init(); 
	// Agrega el recurso con nombre name ubicado en el path dado por path
	// Devuelve true o false y loggea a std::cerr en caso de error
	// La primera version es sin color key, la segunda con
	bool addRes(const std::string& name, const std::string& path);
	bool addRes(const std::string& name, const std::string& path, const int colorKey);
	// Devuelve un puntero a la Surface asociada con name
	// En caso de error devuelve la surface de error
	Surface* getRes(const std::string& name) const;
	// Libera todas las surfaces - se tiene que llamar antes de salir
	void clean();

};