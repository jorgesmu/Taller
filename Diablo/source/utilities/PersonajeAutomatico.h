#pragma once
#include "../utilities/Personaje.h"
#include "../utilities/ImagenPersonaje.h"
#include "../display/tile.h"
class Tile;
class PersonajeAutomatico : public Personaje {

public:

	const static int DELTA_TIEMPO_UPDATE_DIRECCION = 5000 ;

private:

	clock_t tiempoProximoUpdateDireccion;

public:

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	PersonajeAutomatico();
		
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

	*/
	PersonajeAutomatico(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const unsigned int velocidad ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, 
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
					Tile* tile, 
					ResMan& rm , const int colorKey);
	
	/*
		Pre: Se ha creado la instancia.

		Post: Se actualiza el movimiento de la instancia.
	*/
	virtual unsigned int update(Mapa* mapa);
};