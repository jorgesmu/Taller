#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include "resman.h"
#include "../utilities/logErrores.h"

// Ctor
ResMan::ResMan() {
	err_surface = NULL;
	for(auto it = fonts.begin();it != fonts.end();it++)
		*it = NULL;
}

// Dtor
ResMan::~ResMan() {
	//Se podria agregar un clean al destructor para evitar la assertion
	//this -> clean();
	// Checkeo que se hayan liberado todas las surfaces
	assert(surface_map.size() == 0);
}

bool ResMan::init() {
	// Carga la surface de error 
	err_surface = new Surface;
	// Creamos el archivo tmp con la imagen de error
	std::string tmp_path = "err.tmp.bmp";
	std::fstream tmp_file(tmp_path, std::fstream::out | std::fstream::binary | std::fstream::trunc);
	tmp_file.write((const char*)err_img_bmp, sizeof(err_img_bmp));
	tmp_file.close();
	// Leemos y cargamos
	bool res = err_surface->load(tmp_path);
	// Verifica si cargo bien o no
	if(!res) {
		err_log.escribir("Error loading error surface\n");
		delete err_surface;
		err_surface = NULL;
	}
	// Cargamos los fonts
	fonts[0] = new Font;
	if(!fonts[0]->init("../resources/static/font.ttf", Font::SIZE_SMALL)) return false;

	fonts[1] = new Font;
	if(!fonts[1]->init("../resources/static/font.ttf", Font::SIZE_NORMAL)) return false;
	
	fonts[2] = new Font;
	if(!fonts[2]->init("../resources/static/font.ttf", Font::SIZE_BIG)) return false;

}

// Agrega el recurso con nombre name ubicado en el path dado por path
// Devuelve true o false y loggea en caso de error
bool ResMan::addRes(const std::string& name, const std::string& path) {
	// Checkea que el resource con ese nombre no exista
	if(surface_map.find(name) != surface_map.end()) {
		std::stringstream ss;
		ss << "Error, resource <" << name << "> already exists\n";
		err_log.escribir(ss.str());
		return false;
	}
	// Caso contrario intenta cargarlo
	Surface* tmp = new Surface;
	bool load_res = tmp->load(path);
	// Verifica si cargo bien o no
	if(!load_res) {
		std::stringstream ss;
		ss << "Error loading resource <" << name << ">\n";
		err_log.escribir(ss.str());
		return false;
	}else{
		// Guardamos en caso de exito
		surface_map[name] = tmp;
	}
	return true;
}

// Agrega el recurso con nombre name ubicado en el path dado por path
// Devuelve true o false y loggea en caso de error
// Version con color key
bool ResMan::addRes(const std::string& name, const std::string& path, const int colorKey) {
	// Checkea que el resource con ese nombre no exista
	if(surface_map.find(name) != surface_map.end()) {
		std::stringstream ss;
		ss << "Error, resource <" << name << "> already exists\n";
		err_log.escribir(ss.str());
		return false;
	}
	// Caso contrario intenta cargarlo
	Surface* tmp = new Surface;
	bool load_res = tmp->load(path, colorKey);
	// Verifica si cargo bien o no
	if(!load_res) {
		std::stringstream ss;
		ss << "Error loading resource <" << name << ">\n";
		err_log.escribir(ss.str());
		return false;
	}else{
		// Guardamos en caso de exito
		surface_map[name] = tmp;
	}
	return true;
}

// Devuelve un puntero a la Surface asociada con name
// En caso de error devuelve la surface de error (TODO)
Surface* ResMan::getRes(const std::string& name) const {
	if(surface_map.find(name) != surface_map.end()) {
		return surface_map.find(name)->second;
	}else{
		std::stringstream ss;
		ss << "Error, surface <" << name << "> requested\n";
		err_log.escribir(ss.str());
		return err_surface; // Devuelve la surface de error precargada
	}
}

// Libera todas las surfaces - se tiene que llamar antes de salir
void ResMan::clean() {
	// Libera los surfaces de imagenes primero
	for(auto it = surface_map.begin(); it != surface_map.end(); ++it) {
		// Liberamos la memoria asociada 
		it->second->destroy();
		// Borramos del heap
		delete it->second;
	}
	// Borramos todos los elementos ya que los punteros estan liberados
	surface_map.clear();
	// Liberamos la err_surface
	if(err_surface != NULL) {
		err_surface->destroy();
		delete err_surface;
	}
	// Liberamos los fonts
	for(auto it = fonts.begin();it != fonts.end();it++) {
		(*it)->clean();
		delete *it;
	}

}

Font* ResMan::getFont(int size) const {
	// Determinamos el tamaño del font
	Font* ret = NULL;
	switch(size) {
		case Font::SIZE_SMALL:
			ret = fonts[0];
			break;
		case Font::SIZE_NORMAL:
			ret = fonts[1];
			break;
		case Font::SIZE_BIG:
			ret = fonts[2];
			break;
		default:
			std::cerr << "Invalid font-size @ ResMan::getFont()\n";
			break;
	}
	return ret;
}