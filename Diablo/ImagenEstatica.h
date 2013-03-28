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
		Pre: La instancia ha sido creada.

		Post: Se liberan los recursos asociados a la instancia.

	**/
	~ImagenEstatica();
	
	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
		es responsabilidad de esta instancia.
		
		NOTA: En caso de que la instancia no se encuentre en un estado válido se 
		retorna un puntero nulo.
	**/
	virtual Surface* getSurface();
};