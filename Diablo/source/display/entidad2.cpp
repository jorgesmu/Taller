#include "camara.h"
#include "entidad2.h"
#include "resman.h"

Entidad2::Entidad2(){
	u = v = 0;
}

void Entidad2::init(const std::string& name, const ResMan& rm) {
	this->name = name;
	this->surf = rm.getRes(name);
}

void Entidad2::blit(SDL_Surface* dest, const Camara& cam, int x, int y){
	//Blitting
	surf->blit(dest, x + u - (int)cam.getX(), y + v - (int)cam.getY());
}

// Cambia la posicion de referencia la entidad
void Entidad2::offset(int x, int y) {
	u = x;
	v = y;
}