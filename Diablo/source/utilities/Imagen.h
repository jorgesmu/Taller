#pragma once
#include "../utilities/surface.h"

/*
	Una instancia de esta clase permite manejar una imagen determinada, a partir
	de un mapa de bits.
*/
class Imagen {
public:

	const static int ALTO_DEFAULT = 100;

	const static int ANCHO_DEFAULT = 100;

	const static int COLOR_KEY = 0x00000000;

protected:
	
	int alto; //en pixeles
	
	int ancho; //en pixeles

public:
	
	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna el alto de la imagen de la instancia.

	**/
	const int getAlto();

	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna el ancho de la imagen de la instancia.

	**/
	const int getAncho();

	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna un puntero a una imagen en memoria de video, cuya destruccion
		es responsabilidad de esta instancia.

	**/
	virtual Surface* getSurface() = 0;
		
	virtual bool isImagenAnimada() = 0;

protected:

	/**
		Pre: El parametro es un n?ero positivo.

		Post: Se setea el acho de la imagen.
	**/
	void setAncho(const int ancho);

	/**
		Pre: El parametro es un n?ero positivo.

		Post: Se setea el alto de la instancia.
	**/
	void setAlto(const int alto);
	
};