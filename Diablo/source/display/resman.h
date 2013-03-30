#pragma once

#include "../utilities/surface.h"
#include <string>
#include <map>
#include "SDL.h"

class ResMan {
	private:
	std::map<std::string, Surface> surface_map;

	public:
	// Ctor
	ResMan();
	// Dtor
	~ResMan();
	// Carga la surface de error 
	void init(); 
	// Agrega el recurso con nombre name ubicado en el path dado por path
	// Devuelve true o false y loggea a std::cerr en caso de error
	bool addRes(const std::string& name, const std::string& path);
	// Devuelve un puntero a la Surface asociada con name
	// En caso de error devuelve la surface de error
	Surface* getRes(const std::string& name);
	// Libera todas las surfaces - se tiene que llamar antes de salir
	void clean();

};