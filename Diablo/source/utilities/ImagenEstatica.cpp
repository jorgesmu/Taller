#include "ImagenEstatica.h"

/**
	Pre: El parámetro path es una ruta no nula.
	
	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	iniciliza de acuerdo a la imagen dada por el path.
**/
ImagenEstatica::ImagenEstatica(const char* path) {
	this -> surface = new Surface();
	if (this -> surface != NULL) {
		if (this -> surface -> load(path)){
			this -> setAlto(this -> surface -> height());
			this -> setAncho(this -> surface -> width());
		} else {
			this -> setAlto(0);
			this -> setAncho(0);
		}
	} else {
		this -> setAlto(0);
		this -> setAncho(0);
	}
	this -> compartida = false;
}

/**
	Pre: El parámetro path es una ruta no nula.
	
	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	iniciliza de acuerdo a la imagen dada por el path.
**/
ImagenEstatica::ImagenEstatica(const char* path , const int colorKey) {
	this -> surface = new Surface();
	if (this ->surface != NULL) {
		if (this -> surface->load(path , colorKey)){
			this -> setAlto(this -> surface -> height());
			this -> setAncho(this -> surface -> width());
		} else {
			this -> setAlto(0);
			this -> setAncho(0);
		} 
	} else {
		this -> setAlto(0);
		this -> setAncho(0);
	}
	this -> compartida = false;
}

/**
	Pre: -
	
	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	iniciliza de acuerdo a la imagen dada por el path.
**/
ImagenEstatica::ImagenEstatica(const char* name , ResMan& rm) {
	this -> surface = rm.getRes(name);
	if (this -> surface != NULL) {
		this -> setAlto(this -> surface -> height());
		this -> setAncho(this -> surface -> width());
		this -> compartida = true;	
	} else {
		this -> setAlto(0);
		this -> setAncho(0);
		this -> surface = NULL;
		this -> compartida = false;
	}
}

/**
	Pre: -
	
	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	iniciliza de acuerdo a la imagen dada por el path.
**/
ImagenEstatica::ImagenEstatica(const char* name , ResMan& rm, const int ckey) {
	this -> surface = rm.getRes(name);
	if (this -> surface != NULL) {
		this -> setAlto(this -> surface -> height());
		this -> setAncho(this -> surface -> width());
		if (this -> surface -> getSDL_Surface() != NULL){
			SDL_SetColorKey(this -> surface -> getSDL_Surface(), SDL_SRCCOLORKEY, ckey);
		}
		if (this -> surface -> getSDL_SurfaceGris() != NULL){
			SDL_SetColorKey(this -> surface -> getSDL_SurfaceGris(), SDL_SRCCOLORKEY, ckey);
		}
		this -> compartida = true;	
	} else {
		this -> setAlto(0);
		this -> setAncho(0);
		this -> surface = NULL;
		this -> compartida = false;
	}
}

/**
	Pre: La instancia ha sido creada.

	Post: Se liberan los recursos asociados a la instancia.
**/
ImagenEstatica::~ImagenEstatica() {
	if ((!this -> compartida) && (this -> surface != NULL)) {
		this -> surface -> destroy();
		delete(this -> surface);
	}
	this -> surface = NULL;
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
	es responsabilidad de esta instancia.
		
**/
Surface* ImagenEstatica::getSurface(){
	return (this -> surface);
}

bool ImagenEstatica::isImagenAnimada(){
	return false;
}