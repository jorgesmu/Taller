#pragma once
#include "imagen.h"
#include <time.h>
class ImagenAnimada : public Imagen {

protected:

	const static int FPS_DEFAULT = 10;

	const static int DELAY_DEFAULT = 100;

private:

	Surface surfaceOrigen;

	Surface surfaceActual;

	int maxFilas;

	int maxColumnas;

	int filaActual;

	int columnaActual;

	int fps;

	int delay;

	clock_t tiempoFinDelay;

	clock_t tiempoProximoFrame;

	clock_t deltaFrame; //distancia en segundos entre cada frame

public:
	
	/**
		Pre: El parámetro path es una ruta no nula y los parámetros alto y ancho son
		positivos.
	
		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
		iniciliza de acuerdo a la imagen dada por el path.
	**/
	ImagenAnimada(const char* path , int altoSprite , int anchoSprite , 
		const int fps , const int delay , const int colorKey);

	/**
		Pre: La instancia ha sido creada.

		Post: Se liberan los recursos asociados a la instancia.
	**/
	~ImagenAnimada();
};

