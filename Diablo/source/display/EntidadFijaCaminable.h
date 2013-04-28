#pragma once
#include "../display/entidadFija.h"
#include "../utilities/ImagenPersonaje.h"
#include "../display/tile.h"

class EntidadFijaCaminable : public EntidadFija{
	public:
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	EntidadFijaCaminable();
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los parametros.

		NOTA: ImagenEstatica
	*/
	EntidadFijaCaminable(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey);
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	EntidadFijaCaminable(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, Mapa* mapa,
			ResMan& rm , const int colorKey);

	

	EntidadFijaCaminable(EntidadFija* entidadFija);
		
		
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	virtual ~EntidadFijaCaminable();
	
	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile ocupado 
		por dicha instancia.
	*/
	virtual bool isCaminable(Tile* tile , Mapa* mapa);

};

