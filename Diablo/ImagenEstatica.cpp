#include "ImagenEstatica.h"

/**
	Pre: El parámetro path es una ruta no nula.
	
	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	iniciliza de acuerdo a la imagen dada por el path.
**/
ImagenEstatica::ImagenEstatica(const char* path) {
	if (this -> surface.load(path)){
		this -> setAlto(this -> surface.height());
		this -> setAncho(this -> surface.width());
	} else {
		this -> setAlto(0);
		this -> setAncho(0);
		this -> setPath(NULL);
	}
}

/**
	Pre: La instancia ha sido creada.

	Post: Se liberan los recursos asociados a la instancia.
**/
ImagenEstatica::~ImagenEstatica() {
	delete[] this -> getPath();
	this -> setPath(NULL);
	this -> surface.destroy();
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
	es responsabilidad de esta instancia.
		
	NOTA: En caso de que la instancia no se encuentre en un estado válido se 
	retorna un puntero nulo.
**/
SDL_Surface* ImagenEstatica::getSurface(){
	return this -> surface;
