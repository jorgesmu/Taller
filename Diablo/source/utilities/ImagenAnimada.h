#pragma once
#include "Imagen.h"
#include <time.h>
#include "../display/resman.h"

class ImagenAnimada:public Imagen {

public:

	const static int FPS_DEFAULT = 10;

	const static int DELAY_DEFAULT = 100;

protected:

	Surface* surfaceOrigen;

	Surface surfaceActual;

	int maxFilas;

	int maxColumnas;

	int filaActual;

	int columnaActual;

	int fps;

	int delay;

	clock_t tiempoProximoFrame;

	clock_t deltaFrame; //distancia en segundos entre cada frame

	int colorKey;

protected:

	bool compartida;

public:
	
	/**
		Pre: El parametro path es una ruta no nula y los parametros alto y ancho son
		positivos.
	
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenAnimada(const char* path , const int altoSprite , const int anchoSprite , 
				const int fps , const int delay , const int colorKey);

	/**
		Pre: -
	
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenAnimada(const char* name , const int altoSprite , 
				const int anchoSprite , const int fps , const int delay , ResMan& rm ,
				const int colorKey);

	/**
		Pre: La instancia ha sido creada.

		Post: Se liberan los recursos asociados a la instancia.
	**/
	virtual ~ImagenAnimada();

	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna un puntero a una imagen en memoria de video, cuya destruccion
		es responsabilidad de esta instancia.
		
	**/
	virtual Surface* getSurface();
	
	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna la cantidad de FPS.
		
	**/
	int getFPS();

	/**
		Pre: La instancia ha sido creada.

		Post: Se retorna el Delay.
		
	**/
	int getDelay();

protected:

	/*
		Pre: Se han inicilizado todos los parametros.

		Post: Se ha actualizado surfaceActual
	*/
	virtual void nextSprite();
	
};

