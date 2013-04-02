#pragma once
#include "Imagen.h"
#include "surface.h"
#include "../display/resman.h"

class ImagenEstatica:public Imagen {

private:
	
	Surface* surface; //imagen

	bool compartida; //si la imagen esta compartida

public:
	
	/**
		Pre: El parámetro path es una ruta no nula.
		
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenEstatica(const char* path);
	
	/**
		Pre: El parámetro path es una ruta no nula.
		
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenEstatica(const char* path , const int ckey);

	/**
		Pre: -
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenEstatica(const char* name , ResMan& rm);
	
	/**
		Pre: -
		
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenEstatica(const char* name , ResMan& rm, const int ckey);

	/**
		Pre: La instancia ha sido creada.

		Post: Se liberan los recursos asociados a la instancia.

	**/
	virtual ~ImagenEstatica();
	
	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
		es responsabilidad de esta instancia.
		
	**/
	virtual Surface* getSurface();
};