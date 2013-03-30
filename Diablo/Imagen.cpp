#include "Imagen.h"
#include <cstring>

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna un puntero al path de la instancia.

	NOTA: La destruccion de la misma queda a cargo de esta instancia. 

**/
const char* Imagen::getPath() {
	return this -> path;
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna el alto de la imagen de la instancia.

**/
const int Imagen::getAlto() {
	return this -> alto;
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna el ancho de la imagen de la instancia.

**/
const int Imagen::getAncho() {
	return this -> ancho;
}

/**
	Pre: El parametro es un numero positivo.

	Post: Se setea el acho de la imagen.
**/
void Imagen::setAncho(const int ancho) {
	this -> ancho = ancho;
}

/**
	Pre: El parametro es un n?ero positivo.

	Post: Se setea el alto de la instancia.
**/
void Imagen::setAlto(const int alto) {
	this -> alto = alto;
}

/**
	Pre: El parametro es una cadena de caracteres no nula,
	la cual define un path.

	Post: Se setea el path de la instancia con una copia de la candena
	de caracteres pasada por par?etro.
**/
void Imagen::setPath(const char* path) {
	this -> path = NULL;
	if (path != NULL) {
		this -> path = new char[strlen(path) + 1];
		if (this -> path != NULL)
			memcpy(this -> path , path ,strlen(path) + 1);
	}
}