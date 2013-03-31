#include "entidad.h"

Entidad::Entidad(){
}

void Entidad::init(const std::string& name, const std::string& img){
	bool load;
	this -> name = name;
	this -> imagen = img;
	//Surface surface;
	//load = surface.load("../resources/tile.bmp" , Surface::RGB_AZUL);
	this -> surf.load("../resources/tile.bmp" , Surface::RGB_AZUL);;
}

void Entidad::blit(SDL_Surface* dest, const SDL_Rect& cam){
	//Blitting
	surf.blit(dest, cam.x, cam.y);
}