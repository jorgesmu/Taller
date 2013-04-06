#pragma once
#include "../display/entidad.h"
#include "../utilities/ImagenPersonaje.h"
class Personaje : public Entidad {

public:

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	Personaje();
		
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

	*/
	Personaje(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const unsigned int velocidad ,
			const int pixel_ref_x , const int pixel_ref_y,
			const unsigned int posTileX , const unsigned int posTileY , 
			ResMan& rm , const int colorKey);

	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name,
					const unsigned int wTiles , const unsigned int hTiles , 
					const unsigned int fps , const unsigned int delay ,
					const unsigned int altoSprite , const unsigned int anchoSprite ,
					const unsigned int velocidad ,
					const int pixel_ref_x , const int pixel_ref_y,
					const unsigned int posTileX , const unsigned int posTileY , 
					ResMan& rm , const int colorKey);

	/*
		Pre: Los parámetros respetan la siguiente convención:

			"x" e "y": Coordenadas del Tile destino

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
	*/
	virtual void mover(const unsigned int x , const unsigned int y);
};