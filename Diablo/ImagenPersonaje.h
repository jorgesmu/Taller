#pragma once
#include "ImagenAnimada.h"
class ImagenPersonaje :	public ImagenAnimada {

public:

	//Desplazamientos
	const static unsigned int DES_SUR = 0;
	const static unsigned int DES_SURESTE = 1;
	const static unsigned int DES_ESTE = 2;
	const static unsigned int DES_NORESTE = 3; 
	const static unsigned int DES_NORTE = 4;
	const static unsigned int DES_NOROESTE = 5;
	const static unsigned int DES_OESTE = 6;
	const static unsigned int DES_SUROESTE = 7;
	
	//Ataque
	const static unsigned int AT_SUR = 8;
	const static unsigned int AT_SURESTE = 9;
	const static unsigned int AT_ESTE = 10;
	const static unsigned int AT_NORESTE = 11;
	const static unsigned int AT_NORTE = 12;
	const static unsigned int AT_NOROESTE = 13;
	const static unsigned int AT_OESTE = 14;
	const static unsigned int AT_SUROESTE = 15;

	//Estático
	const static unsigned int EST_SUR = 16;
	const static unsigned int EST_SURESTE = 17;
	const static unsigned int EST_ESTE = 18;
	const static unsigned int EST_NORESTE = 19;
	const static unsigned int EST_NORTE = 20;
	const static unsigned int EST_NOROESTE = 21;
	const static unsigned int EST_OESTE = 22;
	const static unsigned int EST_SUROESTE = 23;

	//Muerte
	const static unsigned int MUERTE = 24;

private:

	unsigned int accionActual;
	 
	unsigned int accionSiguiente;

public:	

	/**
		Pre: Condiciones sobre la entrada:
		
			path: no nulo, caso contrario se vuelca a NULL

			altoSprite y anchoSprite: mayores iguales a cero, caso contrario se
			vuelca a default

			fps: mayor que cero, caso contario se vuelca a default

			delay: mayor o igual a cero, caso contrario se vuelca a 0.

			colorKey: cualquier entero para representar un RGB de 32 bits.

		Post: Si se logra abrir el archivo y tomar memoria, la instancia se
			inicializa de acuerdo a la imagen dada por el path.

		Nota: Setea en la acción EST_SUR.
	**/
	ImagenPersonaje(const char* path , const int altoSprite , const int anchoSprite , 
				const int fps , const int delay , const int colorKey);

	virtual ~ImagenPersonaje();

	/*
		Pre: La instancia ha sido creada y el parámetro es una acción entre 0 y 24.

		Post: Se ha seteado la acción.
	*/
	void setAccion(unsigned int accion);
	
	/*
		Pre: Se han inicilizado todos los parámetros.

		Post: Se ha actualizado surfaceActual
	*/
	virtual void nextSprite();
	
};

