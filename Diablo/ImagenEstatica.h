#pragma once
#include "Imagen.h"

class ImagenEstatica:public Imagen {

private:
	
	Surface surface; //imagen

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
		Pre: La instancia ha sido creada.

		Post: Se liberan los recursos asociados a la instancia.

	**/
	~ImagenEstatica();
	
	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
		es responsabilidad de esta instancia.
		
	**/
	virtual Surface* getSurface();
};