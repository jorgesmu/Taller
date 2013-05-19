#include "EntidadFijaCaminable.h"
#include "tile.h"
#include "mapa.h"

	/*
		Pre:-
		 
		Post: Se ha creado la instancia con los valores por defecto.
	*/
	EntidadFijaCaminable::EntidadFijaCaminable() : EntidadFija(){
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	EntidadFijaCaminable::EntidadFijaCaminable(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
					int pixel_ref_x ,int pixel_ref_y,
					Tile* tile, Mapa* mapa,
					ResMan& rm , const int colorKey) : 
						  EntidadFija( name, wTiles , hTiles , caminable,
						               pixel_ref_x , pixel_ref_y,
									   tile, mapa,
									   rm , colorKey){
	}
		
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	EntidadFijaCaminable::EntidadFijaCaminable(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey) : 
	                      EntidadFija( name , wTiles , hTiles , caminable ,
			fps , delay , altoSprite , anchoSprite ,
			pixel_ref_x , pixel_ref_y,
			tile , mapa , rm , colorKey){
	}
	
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	EntidadFijaCaminable::~EntidadFijaCaminable(){
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> surf = NULL;
		this -> tileAncla = NULL;
	}
	
	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile ocupado 
		por dicha instancia.
	*/
	bool EntidadFijaCaminable::isCaminable(Tile* tile , Mapa* mapa){
		return true;
	}

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile ocupado 
		por dicha instancia.
	*/
	bool EntidadFijaCaminable::isCaminable(){
		return true;
	}


	void EntidadFijaCaminable::setColor(bool value , int tileX , int tileY) {
		if ( (this -> widthInTiles > 1) || (this -> highInTiles >1)){
			if ( (this -> posX == tileX) && (this -> posY == tileY)) {
				this -> color = value;	
			}
		} 
	}

	// Deprecated
	void EntidadFijaCaminable::setColor(bool value) {
		this -> color = value;
	}