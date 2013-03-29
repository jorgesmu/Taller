#pragma once
#include "source/utilities/surface.h"

/*
	Una instancia de esta clase permite manejar una imagen determinada, a partir
	de un mapa de bits.
*/
class Imagen {
public:

	const static int ALTO_DEFAULT = 100;

	const static int ANCHO_DEFAULT = 100;

	const static int COLOR_KEY = 0x00000000;

private:
	
	char* path; //path del archivo fuente de la imagen
	
	int alto; //en pixeles
	
	int ancho; //en pixeles

public:
	
	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna un puntero al path de la instancia.

		NOTA: La destrucción de la misma queda a cargo de esta instancia. 
	**/
	const char* getPath();

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

		Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
		es responsabilidad de esta instancia.

	**/
	virtual Surface* getSurface() = 0;
		
protected:

	/**
		Pre: El parámetro es un número positivo.

		Post: Se setea el acho de la imagen.
	**/
	void setAncho(const int ancho);

	/**
		Pre: El parámetro es un número positivo.

		Post: Se setea el alto de la instancia.
	**/
	void setAlto(const int alto);

	/**
		Pre: El parámetro es una cadena de caracteres no nula,
		la cual define un path.

		Post: Se setea el path de la instancia con una copia de la candena
		de caracteres pasada por parámetro.
	**/
	void setPath(const char* path);
};