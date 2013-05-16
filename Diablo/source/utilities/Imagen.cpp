#include "Imagen.h"
#include <cstring>

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
